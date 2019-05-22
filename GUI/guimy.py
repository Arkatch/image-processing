#sudo apt-get install python3-pil.imagetk
#pip install pillow
import os
import imghdr
import tkinter as tk
import platform
from tkinter import *
import tkinter.filedialog as fdialog
from PIL import Image, ImageTk

runtype = ""
if platform.system() == "Linux": runtype = "./"
  
#okno pierwszego i drugiego obrazu`
wind_1 = tk.Tk()
root = tk.Tk()
root.geometry("900x600")
main_filename_opened = StringVar()
main_filename = StringVar()
main_filename = ""
main_filename_opened = ""
main_header = object

class ImgWindow(tk.Frame):
  def __init__(self, master=None, filename=""):
    super().__init__(master)
    self.pack_forget()
    self.master = master
    self.pack()
    image = Image.open(filename, "r")
    render = ImageTk.PhotoImage(image)
    img = Label(self, image=render)
    img.image = render
    img.place(x=0, y=0)
    img.pack()

class Window(tk.Frame):
  def __init__(self, master=None):
    super().__init__(master)
    self.master = master
    self.create_window()
    self.pack()
  
  def create_window(self):
    tk.Button(self, text="Wybierz plik", command=self.load_img).grid(column=0, row=0, sticky="ew")
    self.img_size = tk.Label(self, text="Rozmiar: 0 bajtów")
    self.img_width = tk.Label(self, text="Szerokość: 0 px")
    self.img_height = tk.Label(self, text="Wysokość: 0 px")
    self.img_type = tk.Label(self, text="Typ: brak")
    self.img_size.grid(column=0, row=3, sticky="ew")
    self.img_width.grid(column=0, row=4, sticky="ew")
    self.img_height.grid(column=0, row=5, sticky="ew")
    self.img_type.grid(column=0, row=6, sticky="ew")
    tk.Button(self, text="Reset", command=self.reset_img).grid(column=0, row=7, sticky="ew")
    tk.Button(self, text="Zapisz obraz", command=self.save_img).grid(column=0, row=8, sticky="ew")

    self.closed_text()
    self.create_filter_btn()
    
  def create_filter_btn(self):
    #GRAY
    tk.Label(self, text="Filtry").grid(column=1, row=0, sticky="ew")
    self.status_filter = tk.Label(self, text="Status")
    self.status_filter.grid(column=0, row=2, sticky="ew")
    self.size_filt = tk.Entry(self, textvariable="Name")
    self.size_filt.insert(0, "Rozmiar 3,5,7,9...")
    self.size_filt.grid(column=1, row=1, sticky="ew")
    tk.Button(self, text="Minimum", command=lambda: self.filtr_minmaxmedian(0)).grid(column=1, row=2, sticky="ew")
    tk.Button(self, text="Mediana", command=lambda: self.filtr_minmaxmedian(1)).grid(column=1, row=3, sticky="ew")
    tk.Button(self, text="Maksimum", command=lambda: self.filtr_minmaxmedian(2)).grid(column=1, row=4, sticky="ew")
    tk.Button(self, text="Szum", command=lambda: self.filtr_minmaxmedian(3)).grid(column=1, row=5, sticky="ew")
    
    tk.Label(self, text="Filtry").grid(column=2, row=0, sticky="ew")
    tk.Button(self, text="Wyostrzanie", command=lambda: self.filtrs(0)).grid(column=2, row=1, sticky="ew")
    tk.Button(self, text="Rozmycie", command=lambda: self.filtrs(1)).grid(column=2, row=2, sticky="ew")
    tk.Button(self, text="Rozmycie Gaussa", command=lambda: self.filtrs(2)).grid(column=2, row=3, sticky="ew")
    tk.Button(self, text="Negatyw", command=lambda: self.filtrs(3)).grid(column=2, row=4, sticky="ew")
    tk.Button(self, text="Kontrast", command=lambda: self.filtrs(4)).grid(column=2, row=5, sticky="ew")
    tk.Button(self, text="Pogrubianie", command=lambda: self.filtrs(10)).grid(column=2, row=6, sticky="ew")

    tk.Label(self, text="Krawędzie").grid(column=3, row=0, sticky="ew")
    tk.Button(self, text="Prewitt", command=lambda: self.filtrs(5)).grid(column=3, row=1, sticky="ew")
    tk.Button(self, text="Sobel", command=lambda: self.filtrs(6)).grid(column=3, row=2, sticky="ew")
    tk.Button(self, text="Laplacian", command=lambda: self.filtrs(7)).grid(column=3, row=3, sticky="ew")
    tk.Button(self, text="Szkieletyzacja", command=lambda: self.filtrs(8)).grid(column=3, row=4, sticky="ew")
    tk.Button(self, text="Ścieranie", command=lambda: self.filtrs(9)).grid(column=3, row=5, sticky="ew")

    tk.Label(self, text="Binaryzacja").grid(column=4, row=0, sticky="ew")
    self.treshold = tk.Entry(self, textvariable="Treshold")
    self.treshold.insert(0, "Próg 0-255 | Rozmiar 3,5,7...")
    self.treshold.grid(column=4, row=1, sticky="ew")
    tk.Button(self, text="Binaryzacja", command=lambda: self.binaryzation(0)).grid(column=4, row=2, sticky="ew")
    tk.Button(self, text="Otsu", command=lambda: self.binaryzation(1)).grid(column=4, row=3, sticky="ew")
    tk.Button(self, text="Mediana", command=lambda: self.binaryzation(2)).grid(column=4, row=4, sticky="ew")
    tk.Button(self, text="Bersen", command=lambda: self.binaryzation(3)).grid(column=4, row=5, sticky="ew")
    tk.Button(self, text="Bersen Mediana", command=lambda: self.binaryzation(4)).grid(column=4, row=6, sticky="ew")

    tk.Label(self, text="Segmentacja").grid(column=5, row=0, sticky="ew")
    self.treshold_seg = tk.Entry(self, textvariable="Tresholdseg")
    self.treshold_seg.insert(0, "Próg 0-255")
    self.treshold_seg.grid(column=5, row=1, sticky="ew")
    self.corxy_seg = tk.Entry(self, textvariable="cor")
    self.corxy_seg.insert(0, "x, y")
    self.corxy_seg.grid(column=5, row=2, sticky="ew")
    tk.Button(self, text="Segmentacja", command=self.segmentation).grid(column=5, row=3, sticky="ew")

    #RGB
    tk.Label(self, text="RGB").grid(column=2, row=10, sticky="ew")
    tk.Label(self, text="Filtry").grid(column=1, row=11, sticky="ew")
    self.size_filt24 = tk.Entry(self, textvariable="rgb24filtr")
    self.size_filt24.insert(0, "Rozmiar 3,5,7,9...")
    self.size_filt24.grid(column=1, row=12, sticky="ew")
    tk.Button(self, text="Minimum", command=lambda: self.filtr_minmaxmedian24(0)).grid(column=1, row=13, sticky="ew")
    tk.Button(self, text="Mediana", command=lambda: self.filtr_minmaxmedian24(1)).grid(column=1, row=14, sticky="ew")
    tk.Button(self, text="Maksimum", command=lambda: self.filtr_minmaxmedian24(2)).grid(column=1, row=15, sticky="ew")
    tk.Button(self, text="Średnia", command=lambda: self.filtr_minmaxmedian24(3)).grid(column=1, row=16, sticky="ew")
    tk.Button(self, text="Szum", command=lambda: self.filtr_minmaxmedian24(4)).grid(column=1, row=17, sticky="ew")

    tk.Label(self, text="Krawędzie").grid(column=2, row=11, sticky="ew")
    tk.Button(self, text="Sobel", command=lambda: self.filtrs24(0)).grid(column=2, row=12, sticky="ew")

    tk.Label(self, text="Kolor").grid(column=3, row=11, sticky="ew")
    tk.Button(self, text="Czerwony", command=lambda: self.color24(0)).grid(column=3, row=12, sticky="ew")
    tk.Button(self, text="Zielony", command=lambda: self.color24(1)).grid(column=3, row=13, sticky="ew")
    tk.Button(self, text="Niebieski", command=lambda: self.color24(2)).grid(column=3, row=14, sticky="ew")
    tk.Button(self, text="Szary", command=lambda: self.color24(3)).grid(column=3, row=15, sticky="ew")
    tk.Button(self, text="Szary:Niebieski", command=lambda: self.color24(4)).grid(column=3, row=16, sticky="ew")
    tk.Button(self, text="Szary:Zielony", command=lambda: self.color24(5)).grid(column=3, row=17, sticky="ew")
    tk.Button(self, text="Szary:Czerwony", command=lambda: self.color24(6)).grid(column=3, row=18, sticky="ew")

    tk.Label(self, text="Klasteryzacja").grid(column=4, row=11, sticky="ew")
    self.points_k = tk.Entry(self, textvariable="points_k")
    self.points_k.insert(0, "x1,y1,x2,y2...")
    self.points_k.grid(column=4, row=12, sticky="ew")
    tk.Button(self, text="RGB", command=lambda: self.clustering(0)).grid(column=4, row=13, sticky="ew")
    tk.Button(self, text="GRAY", command=lambda: self.clustering(1)).grid(column=4, row=14, sticky="ew")
  
  def save_img(self):
    if main_filename == "":
      return
    directory = fdialog.asksaveasfilename(filetypes=(("BMP files", "*.bmp"), ("All files", "*.*")))
    if directory:
      filewrite = open(directory, "wb")
      fileread = open(main_filename, "rb")
      file = fileread.read()
      filewrite.write(file)
      fileread.close()
      filewrite.close()
    
  def color24(self, operation):
    if main_filename == "":
      self.status(2)
      return
    code = ""
    if operation == 0: code = "%sline color_mask red_mask %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 1: code = "%sline color_mask green_mask %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 2: code = "%sline color_mask blue_mask %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 3: code = "%sline convert rgb_to_gray %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 4 or operation == 5 or operation == 6:
      code = "%sline convert gray_by_color %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), operation - 4)
    run_command(code, self)
    self.status(0)
  def clustering(self, operation):
    if main_filename == "":
      self.status(2)
      return
    points = str(self.points_k.get()).split(",")
    for i in range(0, len(points)): points[i] = int(points[i])
    size = len(points)/2
    if not (size % 2):
      self.status(6)
      return
    code = ""
    if operation == 0: code = "%sline clustering24 clustering24 %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 1: code = "%sline clustering clustering %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    for i in range(0, len(points), 2):
      x, y = points[i], points[i+1]
      if x < 0 or x >= main_header.width or y < 0 or y >= main_header.height:
        self.status(6)
        return
      code += " "+str(x)+" "+str(y)
      run_command(code, self)
  def segmentation(self):
    treshold = int(str(self.treshold_seg.get()))
    cor = str(self.corxy_seg.get()).split(",")
    cor[0] = int(cor[0])
    cor[1] = int(cor[1])
    if len(cor) != 2 or cor[0] < 0 or cor[1] < 0 or cor[0] >= main_header.width or cor[1] >= main_header.height:
      self.status(6)
      return
    code="%sline growingregion growingregion %s %s %d %d %d"%(runtype, main_filename, os.path.abspath("aaa.bmp"), cor[0], cor[1], treshold)
    run_command(code, self)
    self.status(0)
  def show_info(self, header):
    self.img_size["text"] = "Rozmiar %dKB" % (header.size/1000)
    self.img_width["text"] = "Szerokość %dpx" %(header.width)
    self.img_height["text"] = "Szerokość %dpx" %(header.height)
    if header.size - header.off == header.width * header.height:
      self.img_type["text"] = "Typ 8bit"
    else:
      self.img_type["text"] = "Typ 24bit"
  def status(self, _status):
    if _status == 0:
      self.status_filter["text"] = "Sukces"
      self.status_filter["fg"] = "green"
    if _status == 1:
      self.closed_text()
    if _status == 2:
      self.status_filter["text"] = "Wybierz plik"
      self.status_filter["fg"] = "red"
    if _status == 3:
      self.status_filter["text"] = "Zła wartość progu"
      self.status_filter["fg"] = "red"
    if _status == 4:
      self.status_filter["text"] = "Zły rozmiar"
      self.status_filter["fg"] = "red"
    if _status == 5:
      self.status_filter["text"] = "Od 0 do 100"
      self.status_filter["fg"] = "red"
    if _status == 6:
      self.status_filter["text"] = "Złe współżędne"
      self.status_filter["fg"] = "red"
  def check_type(self):
    if main_header.width * main_header.height == main_header.size - main_header.off:
      return 8
    return 24
  def closed_text(self):
    self.is_open_text = tk.Label(self, text="Brak pliku", fg="red")
    self.is_open_text.grid(column=0, row=1, sticky="ew")
  def open_text(self):
    self.is_open_text = tk.Label(self, text="Otwarto", fg="green")
    self.is_open_text.grid(column=0, row=1, sticky="ew")
  def load_img(self):
    global main_filename_opened
    filename = fdialog.askopenfilename(filetypes=(("Graphic files", ".bmp"), ("All files", "*.*")))
    if filename == "":
      if main_filename_opened == "": 
        self.closed_text()
      return False
    main_filename_opened = filename
    self.draw_image(filename)
  def reset_img(self):
    if main_filename_opened == "":
      return
    self.draw_image(main_filename_opened)
  def draw_image(self, filename):
    header = self.load_bmp_info(filename)
    self.show_info(header)
    global main_header
    global main_filename
    global wind_1
    main_filename = filename
    main_header = header
    self.open_text()
    wind_1.destroy()
    wind_1 = tk.Tk()
    wind_1.geometry(str(header.width)+"x"+str(header.height))
    ImgWindow(master=wind_1, filename=filename)
  def filtr_minmaxmedian(self, operation):
    if main_filename == "":
      self.status(2)
      return
    size = int(str(self.size_filt.get()))
    if operation != 3 and (size < 3 or size % 2 == 0): 
      self.status(4)
      return
    elif operation == 3 and (size < 0 or size > 100):
      self.status(5)
      return
    code = ""
    if operation == 0: code = "%sline image_filter min_template %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 1: code = "%sline image_filter median_template %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 2: code = "%sline image_filter max_template %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 3: code = "%sline other add_salt %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    run_command(code, self)
    self.status(0) 
  def filtrs(self, operation):
    if main_filename == "":
      self.status(2)
      return
    code = ""
    if operation == 0: code = "%sline convolution_matrix sharpen_template %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 1: code = "%sline convolution_matrix boxblur_template %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 2: code = "%sline convolution_matrix gaussianblur_template %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 3: code = "%sline other image_negative %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 4: code = "%sline other contrast_stretch %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 5: code = "%sline edge_detection prewitt_template %s %s""" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 6: code = "%sline edge_detection sobel_template %s %s""" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 7: code = "%sline edge_detection laplacian_template %s %s""" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 8: code = "%sline morphology thinning %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), 0)
    if operation == 9: code = "%sline morphology pruning %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), 0)
    if operation == 10: code = "%sline convolution_matrix dilation_template %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    run_command(code, self)
    self.status(0)
  def binaryzation(self, operation):
    code = ""
    size = int(str(self.treshold.get()))
    if operation == 0 and (size < 0 or size > 255):
      self.status(3)
      return
    elif (operation == 3 or operation == 4) and (size < 3 or size % 2 == 0):
      self.status(4)
      return
    if operation == 0: code = "%sline binarization binarization %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 1: code = "%sline binarization otsumethod %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 2: code = "%sline binarization binarizationmedian %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    if operation == 3: code = "%sline binarization bernsenmethod %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 4: code = "%sline binarization bernsenmethodmedian %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    run_command(code, self)
    self.status(0)
  def load_bmp_info(self, filename):
    file = open(filename, "rb")
    if file == False:
      raise Exception("Can't read file")
    file.seek(0, os.SEEK_END)
    size = file.tell()
    file.seek(0, os.SEEK_SET)
    if size < 54:
      raise Exception("File corrupted")
    #get header from file
    class header:
      bmptype = int.from_bytes(file.read(2),byteorder='little',signed = False)
      size = int.from_bytes(file.read(4),byteorder='little',signed = False)
      reserved1 = int.from_bytes(file.read(2),byteorder='little',signed = False)
      reserved2 = int.from_bytes(file.read(2),byteorder='little',signed = False)
      off = int.from_bytes(file.read(4),byteorder='little',signed = False)
      headerSize = int.from_bytes(file.read(4),byteorder='little',signed = False)
      width = int.from_bytes(file.read(4),byteorder='little',signed = True)
      height = int.from_bytes(file.read(4),byteorder='little',signed = True)
    file.close()
    return header()
  def filtrs24(self, operation):
    if main_filename == "":
      self.status(2)
      return
    code = ""
    if operation == 0:
      code = "%sline edge_detection24 sobel_template %s %s" % (runtype, main_filename, os.path.abspath("aaa.bmp"))
    run_command(code, self)
  def filtr_minmaxmedian24(self, operation):
    if main_filename == "":
      self.status(2)
      return
    size = int(str(self.size_filt24.get()))
    if operation != 4 and (size < 3 or size % 2 == 0): 
      self.status(4)
      return
    elif operation == 4 and (size < 0 or size > 100):
      self.status(5)
      return
    code = ""
    if operation == 0: code = "%sline image_filter24 min_template24 %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 1: code = "%sline image_filter24 median_template24 %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 2: code = "%sline image_filter24 max_template24 %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 3: code = "%sline image_filter24 avg_template24 %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    if operation == 4: code = "%sline other24 add_salt24 %s %s %d" % (runtype, main_filename, os.path.abspath("aaa.bmp"), size)
    run_command(code, self)
    self.status(0) 

def run_command(action, window):
  os.system(action)
  path = os.path.abspath("aaa.bmp")
  window.draw_image(path)

app = Window(master=root)
app.mainloop()
