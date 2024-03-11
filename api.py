from flask import Flask, jsonify, request
import os
from IPython import display
from ultralytics import YOLO
from PIL import Image
import cv2
import numpy as np
import torch

app = Flask(__name__)

BASE_OUTPUT = '/content/data'
seg_model = YOLO('/content/yolo-segment.pt')
seg_model.eval()

@app.route('/process_video', methods=['POST'])
def process_video():
    try:
        video_file = request.files['video']
        num_images = int(request.form['num_images'])
        
        video_path = os.path.join(BASE_OUTPUT, 'input_video.mp4')
        video_file.save(video_path)

        segmented_image_path = os.path.join(BASE_OUTPUT, 'segmented/input')
        get_images_from_video(video_path, segmented_image_path, num_images=num_images)

        data_path = os.path.join(BASE_OUTPUT, 'segmented')
        convert_to_density_map(data_path)

        refine_density_map(data_path)

        # Zip mesh files
        zip_mesh_files(data_path)

        return jsonify({'status': 'success'})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})
        
def run_python_script(script_path, arguments=[]):
    command = ['python', script_path] + arguments
    try:
        subprocess.run(command, check=True)
        print(f"Script '{script_path}' executed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error executing script '{script_path}': {e}")

def get_images_from_video(video_path, segmented_image_path, num_images=60):
    if not os.path.exists(OUT_PATH):
        os.makedirs(OUT_PATH)

    vidName = str(IN_PATH.split('/')[-1].split(".")[0])
#     print(vidName)
    cap = cv2.VideoCapture(IN_PATH)

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    frame_interval = max(1, total_frames // num_images)  # Ensure at least one frame is extracted
#     print(frame_interval)
    count = 0
    success = True
    while success:
        success, image = cap.read()
        if count % frame_interval == 0:
            img_filename = f'{OUT_PATH}/{vidName}_{count}.jpg'
            if success:
#                 print(img_filename)
                results = seg_model(image, retina_masks=True, iou=0.5, verbose=False)
                if results[0].masks is None:
                    mask = torch.zeros(1, results[0].orig_img.shape[0], results[0].orig_img.shape[1])
                else:
                    mask = results[0].masks.data
                mask = mask.permute(1, 2, 0).cpu().numpy()
                mask = mask[:,:,0]
                segmented = get_segmented_image(image, mask)
                cv2.imwrite(img_filename, segmented)
        count += 1

    cap.release()

def convert_to_density_map(data_path):
    run_python_script('/content/SuGaR/gaussian_splatting/convert.py', arguments=['-s', data_path, '--no_gpu'])

def refine_density_map(data_path):
    run_python_script('/content/SuGaR/gaussian_splatting/train.py', arguments=['-s', data_path, '--iteration', '7000'])
    datapath = '/content/output/' + os.listdir('/content/output')[0] + '/'
    os.chdir('/content/SuGaR')
    run_python_script('train.py', arguments=['-s', data_path, '-c', datapath, '-r', 'density', '--refinement_iterations', '9000', '--	refinement_time', 'short', '--eval', 'False', '--low_poly', 'True'])

def zip_mesh_files(data_path):
    !zip -r /content/mesh.zip /content/SuGaR/output/refined_mesh/segmented/*

if __name__ == '__main__':
    app.run(debug=True)

