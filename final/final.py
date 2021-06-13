# Edge Impulse - OpenMV Image Classification Example

import sensor, image, time, os, tf, pyb, math


THRESHOLD = (20,95)
enable_lens_corr = True # turn on for straighter lines...
sensor.reset()                         # Reset and initialize the sensor.
sensor.set_pixformat(sensor.GRAYSCALE)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)
#sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)


f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)

net = "trained.tflite"
labels = [line.rstrip('\n') for line in open("labels.txt")]


uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

def degrees(radians):
   return (180 * radians) / math.pi

while(True):
   flag = 0
   distance = 0
   deg = 0
   sign = 0
   t_x = 0
   sign2 = 0
   line_true = 'N'
   tag_true = 'N'
   diff_abs = 0
   sign3 = 0
   label = 0
   tag_id = 1
   img = sensor.snapshot()

   for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11
      flag = 1
      distance = 6.2 * (tag.x_translation()**2 +  tag.z_translation()**2)**0.5
      deg = degrees(tag.y_rotation())
      t_x = 6.2*tag.x_translation()
      if deg > 180: deg = deg - 360
      if deg > 0: sign = 1
      else :
         sign = 2
         deg = -deg
      if t_x >= 0: sign2 = 1
      else :
        sign2 = 2
        t_x = -t_x
      tag_true = 'Y'
      tag_id = tag.id()
   if flag == 0: tag_true = 'N'

   for obj in tf.classify(net, img, min_scale=1.0, scale_mul=0.8, x_overlap=0.5, y_overlap=0.5):
      predictions_list = list(zip(labels, obj.output()))

      if predictions_list[0][1]> predictions_list[1][1]: label = 0
      else: label = 1

   img2 = sensor.snapshot().binary([THRESHOLD])
   if enable_lens_corr: img2.lens_corr(1.8) # for 2.8mm lens...
   line = img2.get_regression([(255,255)], robust = True,roi=(10, 0, 140, 45))
   if (line) and (line.magnitude()>10):
      diff = line.x2() - line.x1()
      if diff <= 0:
         diff_abs = -diff
         sign3 = 0
      else :
         diff_abs = diff
         sign3 = 1
      line_true = 'y'
   else : line_true = 'n'

   print_args = (tag_true, int(tag_id), int(distance), int(deg), sign, int(t_x), sign2, line_true, int(diff_abs), int(sign3), int(label))
   print("%c,%03d,%03d,%03d,%d,%03d,%d/%c,%03d,%d/%d\r\n" % print_args)
   uart.write(("%c,%03d,%03d,%03d,%d,%03d,%d/%c,%03d,%d/%d\r\n" % print_args).encode())
   time.sleep(0.2)
