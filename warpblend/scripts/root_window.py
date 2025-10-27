from tkinter import *
from calc_projector_screen import *
import params
import warpblend_window

root = Tk()

def quit_root_window():
   root.destroy()

def create_root_window():

    root.geometry('{}x{}'.format(params.ROOT_NX, params.ROOT_NY))
    root.title("Main Window")


    # Create a Quit Button
    root_button=Button(root,text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root_window)

    # Place the button in the window
    root_button.grid(row=0, column=2, padx=0, pady=0, sticky="n")

    # Create a label
    label = Label(root, text="R [cm] ")
    label.grid(row=1, column=0, padx=10, pady=10, sticky="e")

    # Create an entry field
    entry = Entry(root, width=20)
    entry.grid(row=1, column=1, padx=10, pady=10)

    entry.insert(0,params.R)

    # Button to read input
    def recalc_grid():
        params.R = float(entry.get())
        print(params.R)
        for mon in range(0,params.nmon,1):
            warpblend_window.redraw_warpblend_window(mon)


    recalc_button = Button(root, text="Recalc", command=recalc_grid)
    recalc_button.grid(row=5, column=2, columnspan=2, pady=10)
