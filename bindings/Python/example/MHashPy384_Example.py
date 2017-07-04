##################################################################################################
# MHash-384 - Language bindings for Python                                                       #
# Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                            #
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
import time

from tkinter import *
from tkinter import filedialog
from tkinter.ttk import *
from os import path
from queue import Queue
from queue import Empty
from threading import Thread

from MHashPy384 import MHash384


#############################################################################
# HASH COMPUTATION (THREADED)
#############################################################################

def get_file_size(input_file):
    try:
        return path.getsize(input_file)
    except:
        return 0

def read_chunks(fs, chunk_size=4096):
    while True:
        data = fs.read(chunk_size)
        if not data:
            break
        yield data

def compute_status(current, maximum):
    return round(100.0 * (float(current) / float(maximum)))

def thread_main(text_out, input_file, queue):
    try:
        bytes_done = update_int = status_old = status_new = 0
        total_size = get_file_size(input_file)
        mhash384 = MHash384()
        with open(input_file, 'rb') as fs:
            for chunk in read_chunks(fs):
                mhash384.update(chunk)
                bytes_done += len(chunk)
                update_int += 1
                if total_size > 0 and update_int >= 97:
                    status_new = compute_status(bytes_done, total_size)
                    if status_new > status_old:
                        queue.put(int(status_new))
                        status_old = status_new
                        time.sleep(0.01)
                    update_int = 0
            queue.put(binascii.hexlify(mhash384.digest()))
    except:
        queue.put("Error: Something went wrong!")


#############################################################################
# CALLBACK FUNCTIONS
#############################################################################

def set_buttons_enabled(root, enable):
    for child in root.winfo_children():
        if isinstance(child, Button):
            child.configure(state='normal' if enable else 'disabled')

def check_status(root, queue, text_out, progress):
    try:
        qval = queue.get_nowait()
        if isinstance(qval, int):
            progress.set(qval)
            root.after(32, check_status, root, queue, text_out, progress)
        else:
            text_out.set(qval)
            progress.set(100)
            set_buttons_enabled(root, True)
    except Empty:
        root.after(250, check_status, root, queue, text_out, progress)

def compute_digest(root, text_out, input_file, progress):
    queue = Queue(maxsize=8)
    text_out.set("Working, please wait...")
    progress.set(0)
    set_buttons_enabled(root, False)
    thread = Thread(target=thread_main, args=(text_out, input_file, queue))
    thread.start()
    root.after(250, check_status, root, queue, text_out, progress)

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

def make_spacer(root, size):
    return Canvas(root, height=size, bd=0, highlightthickness=0)

def initialize_gui():
    root = Tk()
    root.wm_title("MHashPy384 - Example App v%d.%d.%d" % MHash384.version())
    center_window(root, 800, 256)
    root.update()
    root.minsize(root.winfo_width(), root.winfo_height())
    progress = IntVar()
    text_file = StringVar()
    text_hash = StringVar()
    make_spacer(root, 8).pack(fill=X)
    Label(root, text="Input file:", anchor="w").pack(fill=X, padx=8)
    Entry(root, state="readonly", textvariable=text_file).pack(fill=X, padx=8)
    make_spacer(root, 20).pack(fill=X)
    Label(root, text="File Digest:", anchor="w").pack(fill=X, padx=8)
    Entry(root, state="readonly", textvariable=text_hash).pack(fill=X, padx=8)
    make_spacer(root, 30).pack(fill=X)
    Progressbar(root, variable=progress).pack(fill=X, padx=8)
    Button(root, text="Compute Hash", command=lambda: \
        compute_digest(root, text_hash, text_file.get(), progress)) \
        .pack(fill=X, side=RIGHT, anchor="s", padx=8, pady=8)
    Button(root, text="Browse File", command=lambda: \
        browse_for_file(root, text_hash, text_file)) \
        .pack(fill=X, side=RIGHT, anchor="s", padx=8, pady=8)


#############################################################################
# MAIN
#############################################################################

def main():
    initialize_gui()
    mainloop()

if __name__ == "__main__":
    main()
