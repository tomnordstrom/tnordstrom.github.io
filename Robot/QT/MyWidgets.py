# This Python file uses the following encoding: utf-8
from PySide2.QtWidgets import QApplication, QMainWindow, QDialog, QLabel, QGridLayout, QPushButton, QWidget, QStyleFactory,QLineEdit

class MyQLineEdit(QLineEdit):
    def __init__(self, parent):
        QLineEdit.__init__(self, parent)
        print("init lineedit")
    #def focusInEvent(self, e):
    #    super(QLineEdit, self).focusInEvent(e)
    #    try:
    #        print("opening keyboard")
    #    except FileNotFoundError:
    #        pass
    def mousePressEvent(self, event):
        #QLineEdit.mousePressEvent(event)
                # open the link on your browser
        print("clicked")
    #def focusOutEvent(self,e):
    #    subprocess.Popen(["killall","matchbox-keyboard"])

