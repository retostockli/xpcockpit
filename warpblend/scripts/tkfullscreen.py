from tkinter import *
import random

# --- constants --- (UPPER_CASE names)

SCREEN_WIDTH  = 1920
SCREEN_HEIGHT = 1080

DISTANCE = 50
NUMBER = (SCREEN_WIDTH // DISTANCE) + 1

# --- functions --- (lower_case names)

def move():
    # remove first
    data.pop(0)

    # add last
    data.append(random.randint(0,SCREEN_HEIGHT))

    # remove all lines
    canvas.delete('all')

    # draw new lines        
    for x, (y1, y2) in enumerate(zip(data, data[1:])):
        x1 = x * DISTANCE
        x2 = (x+1) * DISTANCE  # x1 + DISTANCE
        canvas.create_line([x1, y1, x2, y2])

    # run again after 500ms (0.5s)
    root.after(500, move)

# --- main --- (lower_case names)

# data at start
data = [random.randint(0, SCREEN_HEIGHT) for _ in range(NUMBER)]


root = Tk()
root.geometry('{}x{}'.format(SCREEN_WIDTH, SCREEN_HEIGHT))

#Full Screen Window
root.attributes('-fullscreen', True)

def quit_root():
   root.destroy()

# Create a Quit Button
button=Button(root,text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root)
button.pack(pady=20)

canvas = Canvas(root, width=SCREEN_WIDTH, height=SCREEN_HEIGHT)
canvas.pack()


# start animation    
move()

root.mainloop()
