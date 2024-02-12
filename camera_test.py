import cv2


source = cv2.VideoCapture("/dev/video2")


while True:
    _, frame = source.read()

    cv2.imshow("frame", frame)
    if cv2.waitKey(1) and 0xFF == ord('q'):
        break


source.release()
cv2.destroyAllWindows()