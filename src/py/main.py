#!/usr/bin/env python3

from PyQt5 import QtCore, QtWidgets
from PyQt5.QtWidgets import QApplication

import sys
import signal

def sigint_handler(*args):
	print("Sigint!")
	QApplication.quit()

class QtApp(QtCore.QObject):
	def __init__(self, app):
		super().__init__()
		self.app = app

if __name__ == "__main__":
	signal.signal(signal.SIGINT, signal.SIG_DFL)
	qapp = QtWidgets.QApplication(sys.argv)
	myapp = QtApp(qapp)
	qapp.exec()

