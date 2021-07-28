# -*- coding: utf-8 -*-
# !/usr/bin/python3
# HiLens Framework 0.2.2 python demo

import cv2
# import socket
import threading
import time
import os
import hilens
from utils import preprocess
from utils import get_socket
from utils import get_result
from utils import draw_boxes

res = None
has_mask = "2".encode("utf-8")
not_mask = "1".encode("utf-8")
unknown = "0".encode("utf-8")
#
def sock_com():
    c = get_socket()
    while True:
        _ = c.recv(1)
        result = 0
        for i in range(10):
            time.sleep(0.1)
            if res is None:
                c.send("5".encode("utf-8"))
                continue
            else:
                result += res/10
        if result > 1.4:
            c.send(has_mask)
        elif result > 0.4:
            c.send(not_mask)
        else:
            c.send(unknown)
            print("unknown00000000000000000")

def run(work_path):
    # 系统初始化，参数要与创建技能时填写的检验值保持一致
    hilens.init("mask")
    # c = get_socket()
    # while True:
    #     data = c.recv(5)
    t = threading.Thread(target=sock_com)
    t.start()
    # 初始化自带摄像头与HDMI显示器,
    # hilens studio中VideoCapture如果不填写参数，则默认读取test/camera0.mp4文件，在hilens kit中不填写参数则读取本地摄像头
    camera = hilens.VideoCapture()
    display = hilens.Display(hilens.HDMI)

    # 初始化模型
    mask_model_path = os.path.join(work_path, 'model/mask_det.om')
    mask_model = hilens.Model(mask_model_path)

    while True:
        try:
            #time.sleep(1)
            ##### 1. 设备接入 #####
            input_yuv = camera.read()  # 读取一帧图片(YUV NV21格式)

            ##### 2. 数据预处理 #####
            img_rgb = cv2.cvtColor(input_yuv, cv2.COLOR_YUV2RGB_NV21)  # 转为RGB格式
            img_preprocess, img_w, img_h = preprocess(img_rgb)  # 缩放为模型输入尺寸

            ##### 3. 模型推理 #####
            output = mask_model.infer([img_preprocess.flatten()])

            ##### 4. 结果输出 #####
            bboxes,res = get_result(output, img_w, img_h)  # 获取检测结果
            print(bboxes)
            img_rgb = draw_boxes(img_rgb, bboxes)      # 在图像上画框
            output_yuv = hilens.cvt_color(img_rgb, hilens.RGB2YUV_NV21)
            display.show(output_yuv)  # 显示到屏幕上
        except Exception:
            hilens.info('last frame~~~')
            break

    hilens.terminate()
    t.join()


if __name__ == "__main__":
    run(os.getcwd())