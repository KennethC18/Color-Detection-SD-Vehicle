import numpy as np
import os
import configparser
import sys
import cv2
import wget
import serial

def download_calibration_file(serial_number):
    if os.name == 'nt':
        hidden_path = os.getenv('APPDATA') + '\\Stereolabs\\settings\\'
    else:
        hidden_path = '/usr/local/zed/settings/'
    calibration_file = hidden_path + 'SN' + str(serial_number) + '.conf'

    if not os.path.isfile(calibration_file):
        url = 'http://calib.stereolabs.com/?SN='
        filename = wget.download(url=url+str(serial_number), out=calibration_file)
        if not os.path.isfile(calibration_file):
            print('Invalid Calibration File')
            return ""
    return calibration_file

def init_calibration(calibration_file, image_size):
    cameraMarix_left = cameraMatrix_right = map_left_y = map_left_x = map_right_y = map_right_x = np.array([])
    config = configparser.ConfigParser()
    config.read(calibration_file)
    check_data = True
    resolution_str = ''
    if image_size.width == 2208:
        resolution_str = '2K'
    elif image_size.width == 1920:
        resolution_str = 'FHD'
    elif image_size.width == 1280:
        resolution_str = 'HD'
    elif image_size.width == 672:
        resolution_str = 'VGA'
    else:
        resolution_str = 'HD'
        check_data = False

    T_ = np.array([-float(config['STEREO']['Baseline'] if 'Baseline' in config['STEREO'] else 0),
                   float(config['STEREO']['TY_'+resolution_str] if 'TY_'+resolution_str in config['STEREO'] else 0),
                   float(config['STEREO']['TZ_'+resolution_str] if 'TZ_'+resolution_str in config['STEREO'] else 0)])

    left_cam_cx = float(config['LEFT_CAM_'+resolution_str]['cx'] if 'cx' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_cy = float(config['LEFT_CAM_'+resolution_str]['cy'] if 'cy' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_fx = float(config['LEFT_CAM_'+resolution_str]['fx'] if 'fx' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_fy = float(config['LEFT_CAM_'+resolution_str]['fy'] if 'fy' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_k1 = float(config['LEFT_CAM_'+resolution_str]['k1'] if 'k1' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_k2 = float(config['LEFT_CAM_'+resolution_str]['k2'] if 'k2' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_p1 = float(config['LEFT_CAM_'+resolution_str]['p1'] if 'p1' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_p2 = float(config['LEFT_CAM_'+resolution_str]['p2'] if 'p2' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_p3 = float(config['LEFT_CAM_'+resolution_str]['p3'] if 'p3' in config['LEFT_CAM_'+resolution_str] else 0)
    left_cam_k3 = float(config['LEFT_CAM_'+resolution_str]['k3'] if 'k3' in config['LEFT_CAM_'+resolution_str] else 0)

    right_cam_cx = float(config['RIGHT_CAM_'+resolution_str]['cx'] if 'cx' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_cy = float(config['RIGHT_CAM_'+resolution_str]['cy'] if 'cy' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_fx = float(config['RIGHT_CAM_'+resolution_str]['fx'] if 'fx' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_fy = float(config['RIGHT_CAM_'+resolution_str]['fy'] if 'fy' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_k1 = float(config['RIGHT_CAM_'+resolution_str]['k1'] if 'k1' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_k2 = float(config['RIGHT_CAM_'+resolution_str]['k2'] if 'k2' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_p1 = float(config['RIGHT_CAM_'+resolution_str]['p1'] if 'p1' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_p2 = float(config['RIGHT_CAM_'+resolution_str]['p2'] if 'p2' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_p3 = float(config['RIGHT_CAM_'+resolution_str]['p3'] if 'p3' in config['RIGHT_CAM_'+resolution_str] else 0)
    right_cam_k3 = float(config['RIGHT_CAM_'+resolution_str]['k3'] if 'k3' in config['RIGHT_CAM_'+resolution_str] else 0)

    R_zed = np.array([float(config['STEREO']['RX_'+resolution_str] if 'RX_' + resolution_str in config['STEREO'] else 0),
                      float(config['STEREO']['CV_'+resolution_str] if 'CV_' + resolution_str in config['STEREO'] else 0),
                      float(config['STEREO']['RZ_'+resolution_str] if 'RZ_' + resolution_str in config['STEREO'] else 0)])

    R, _ = cv2.Rodrigues(R_zed)
    cameraMatrix_left = np.array([[left_cam_fx, 0, left_cam_cx],
                                 [0, left_cam_fy, left_cam_cy],
                                 [0, 0, 1]])

    cameraMatrix_right = np.array([[right_cam_fx, 0, right_cam_cx],
                                  [0, right_cam_fy, right_cam_cy],
                                  [0, 0, 1]])

    distCoeffs_left = np.array([[left_cam_k1], [left_cam_k2], [left_cam_p1], [left_cam_p2], [left_cam_k3]])
    distCoeffs_right = np.array([[right_cam_k1], [right_cam_k2], [right_cam_p1], [right_cam_p2], [right_cam_k3]])

    T = np.array([[T_[0]], [T_[1]], [T_[2]]])
    R1 = R2 = P1 = P2 = np.array([])

    R1, R2, P1, P2 = cv2.stereoRectify(cameraMatrix1=cameraMatrix_left,
                                       cameraMatrix2=cameraMatrix_right,
                                       distCoeffs1=distCoeffs_left,
                                       distCoeffs2=distCoeffs_right,
                                       R=R, T=T,
                                       flags=cv2.CALIB_ZERO_DISPARITY,
                                       alpha=0,
                                       imageSize=(image_size.width, image_size.height),
                                       newImageSize=(image_size.width, image_size.height))[0:4]

    map_left_x, map_left_y = cv2.initUndistortRectifyMap(cameraMatrix_left, distCoeffs_left, R1, P1, (image_size.width, image_size.height), cv2.CV_32FC1)
    map_right_x, map_right_y = cv2.initUndistortRectifyMap(cameraMatrix_right, distCoeffs_right, R2, P2, (image_size.width, image_size.height), cv2.CV_32FC1)

    cameraMatrix_left = P1
    cameraMatrix_right = P2

    return cameraMatrix_left, cameraMatrix_right, map_left_x, map_left_y, map_right_x, map_right_y

class Resolution:
    width = 1280
    height = 720

def detect_arrows(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    edges = cv2.Canny(blurred, 50, 150)
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    arrow_boxes = []
    arrow_detected = 0
    
    for contour in contours:
        if cv2.contourArea(contour) < 200:
            continue
        epsilon = 0.02 * cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, epsilon, True)
        
        if len(approx) == 7:
            x, y, w, h = cv2.boundingRect(contour)
            aspect_ratio = w / float(h)
            if 0.5 < aspect_ratio < 2.0:
                arrow_boxes.append((x, y, w, h))
                arrow_detected = 1
    
    return arrow_boxes, arrow_detected

def try_open_serial(ports, baudrate=9600):
    for port in ports:
        try:
            ser = serial.Serial(
                port=port,
                baudrate=baudrate,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=1
            )
            print(f"UART initialized on {port}")
            return ser
        except serial.SerialException as e:
            print(f"Failed to open {port}: {e}")
    return None

def main():
    if len(sys.argv) == 1:
        print('Please provide ZED serial number')
        exit(1)

    # Initialize UART
    ports = ['/dev/ttyAMA0', '/dev/serial0', '/dev/ttyS0']
    ser = try_open_serial(ports)
    if ser is None:
        print("Error: Could not open any UART port.")
        exit(1)

    # Open the ZED camera
    cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
    if not cap.isOpened():
        ser.close()
        exit(-1)

    image_size = Resolution()
    image_size.width = 1280
    image_size.height = 720

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, image_size.width*2)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, image_size.height)

    serial_number = int(sys.argv[1])
    calibration_file = download_calibration_file(serial_number)
    if not calibration_file:
        ser.close()
        cap.release()
        exit(1)
    print("Calibration file found. Loading...")

    camera_matrix_left, camera_matrix_right, map_left_x, map_left_y, map_right_x, map_right_y = init_calibration(calibration_file, image_size)

    # Define HSV color ranges
    color_ranges = {
        'red': [(np.array([0, 120, 70]), np.array([10, 255, 255])),
                (np.array([170, 120, 70]), np.array([180, 255, 255]))],
        'blue': [(np.array([100, 120, 70]), np.array([130, 255, 255]))],
        'green': [(np.array([40, 120, 70]), np.array([80, 255, 255]))]
    }

    # Maximum area for scaling (adjust based on expected object sizes at 640x360)
    max_area = 10000  # Example: 100x100 pixels

    while True:
        ret, frame = cap.read()
        if not ret:
            break
        left, _ = np.split(frame, 2, axis=1)  # Use only left image
        left_small = cv2.resize(left, (640, 360))
        
        # Color detection
        hsv = cv2.cvtColor(left_small, cv2.COLOR_BGR2HSV)
        output_frame = left_small.copy()  # Needed for processing, not displayed

        areas = {'red': 0, 'green': 0, 'blue': 0}  # 0 indicates not detected

        for color_name, ranges in color_ranges.items():
            mask_color = np.zeros_like(hsv[:, :, 0])
            for lower, upper in ranges:
                mask_color |= cv2.inRange(hsv, lower, upper)
            
            contours, _ = cv2.findContours(mask_color, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            for contour in contours:
                if cv2.contourArea(contour) > 100:
                    x, y, w, h = cv2.boundingRect(contour)
                    area = w * h  # Area in pixels
                    # Scale area to 0-255
                    scaled_area = min(int((area / max_area) * 255), 255)
                    areas[color_name] = scaled_area
                    # Draw bounding box and labels (commented out for performance)
                    # color_bgr = {'red': (0, 0, 255), 'blue': (255, 0, 0), 'green': (0, 255, 0)}[color_name]
                    # cv2.rectangle(output_frame, (x, y), (x+w, y+h), color_bgr, 2)
                    # cv2.putText(output_frame, color_name, (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color_bgr, 2)
                    # cv2.putText(output_frame, f"{scaled_area}", (x, y+h+20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color_bgr, 2)

        # Arrow detection (for potential future use, not sent via UART)
        arrow_boxes, _ = detect_arrows(left_small)
        # for (x, y, w, h) in arrow_boxes:
        #     cv2.rectangle(output_frame, (x, y), (x+w, y+h), (0, 255, 255), 2)
        #     cv2.putText(output_frame, 'Arrow', (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 255), 2)

        # Create and send UART frame with fixed-length scaled areas (000-255)
        frame = f"{areas['red']:03d},{areas['green']:03d},{areas['blue']:03d}\n"
        try:
            ser.write(frame.encode('ascii'))
            print(f"Sent UART frame: {frame.strip()}")
        except serial.SerialException as e:
            print(f"Failed to send UART frame: {e}")

        # Display results (commented out for performance)
        # cv2.imshow("Color and Arrow Detection", output_frame)
        # key = cv2.waitKey(30)
        # if key >= 0:
        #     break

    ser.close()
    cap.release()
    # cv2.destroyAllWindows()  # Not needed since no windows are opened
    exit(0)

if __name__ == "__main__":
    main()
