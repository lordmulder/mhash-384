##################################################################################################
# MHash-384 - Language bindings for Python                                                       #
# Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 #
#                                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software  #
# and associated documentation files (the "Software"), to deal in the Software without           #
# restriction, including without limitation the rights to use, copy, modify, merge, publish,     #
# distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  #
# Software is furnished to do so, subject to the following conditions:                           #
#                                                                                                #
# The above copyright notice and this permission notice shall be included in all copies or       #
# substantial portions of the Software.                                                          #
#                                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  #
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     #
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   #
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        #
##################################################################################################

import sys
import binascii

from MHashPy384_Wrapper import MHash384

from tkinter import *
from tkinter import ttk
from tkinter import filedialog

from queue import Queue
from queue import Empty

from threading import Thread


#############################################################################
# HASH COMPUTATION (THREADED)
#############################################################################

def read_chunks(fs, chunk_size=4096):
    while True:
        data = fs.read(chunk_size)
        if not data:
            break
        yield data

def thread_main(text_out, input_file, queue):
    try:
        with MHash384() as digest:
            with open(input_file, 'rb') as fs:
                for chunk in read_chunks(fs):
                    digest.update(chunk)
                queue.put(binascii.hexlify(digest.result()))
    except:
        queue.put("Error: Something went wrong!")


#############################################################################
# CALLBACK FUNCTIONS
#############################################################################

def set_buttons_enabled(root, enable):
    for child in root.winfo_children():
        if isinstance(child, Button):
            child.configure(state='normal' if enable else 'disabled')

def check_status(root, queue, text_out):
    try:
        text_out.set(queue.get_nowait())
        set_buttons_enabled(root, True)
    except Empty:
        root.after(250, check_status, root, queue, text_out)

def compute_digest(root, text_out, input_file):
    queue = Queue()
    thread = Thread(target=thread_main, args=(text_out, input_file, queue))
    set_buttons_enabled(root, False)
    text_out.set("Working, please wait...")
    thread.start()
    root.after(250, check_status, root, queue, text_out)

def browse_for_file(root, text_hash, text_file):
    file_name = filedialog.askopenfilename(parent=root, title='Choose a file')
    if file_name:
        text_hash.set("")
        text_file.set(file_name)


#############################################################################
# GUI FUNCTIONS
#############################################################################

def center_window(root, width, height):
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    x = (screen_width - width) / 2
    y = (screen_height - height) / 2
    root.geometry('%dx%d+%d+%d' % (width, height, x, y))

def initialize_gui():
    root = Tk()
    center_window(root, 768, 192)
    root.wm_title("MHashPy384 - Example App v%d.%d.%d" % MHash384.getver())
    root.update()
    root.minsize(root.winfo_width(), root.winfo_height())
    text_file = StringVar()
    text_hash = StringVar()
    Frame(root, height=8).pack(fill=X)
    Label(root, text="Input file:", anchor="w").pack(fill=X, padx=8)
    Entry(root, state="readonly", textvariable=text_file).pack(fill=X, padx=8)
    Frame(root, height=20).pack(fill=X)
    Label(root, text="File Digest:", anchor="w").pack(fill=X, padx=8)
    Entry(root, state="readonly", textvariable=text_hash).pack(fill=X, padx=8)
    Button(root, text="Compute Hash", padx=16, command=lambda: compute_digest(root, text_hash, text_file.get())).pack(fill=X, side=RIGHT, anchor="s", padx=8, pady=8)
    Button(root, text="Brose File", padx=16, command=lambda: browse_for_file(root, text_hash, text_file)).pack(fill=X, side=RIGHT, anchor="s", padx=8, pady=8)


#############################################################################
# MAIN
#############################################################################

def main():
    initialize_gui()
    mainloop()

if __name__ == "__main__":
    main()
