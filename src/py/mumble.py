#
# Copyright (c) 2020 smeat.
#
# This file is part of GW2Overlay 
# (see https://github.com/Smeat/GW2Overlay).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
import ctypes
import mmap
import socket
import time


class Link(ctypes.Structure):
	_fields_ = [
		("uiVersion", ctypes.c_uint32),		   # 4 bytes
		("uiTick", ctypes.c_uint32),			   # 4 bytes
		("fAvatarPosition", ctypes.c_float * 3),  # 3*4 bytes
		("fAvatarFront", ctypes.c_float * 3),	 # 3*4 bytes
		("fAvatarTop", ctypes.c_float * 3),	   # 3*4 bytes
		("name", ctypes.c_wchar * 256),		   # 512 bytes
		("fCameraPosition", ctypes.c_float * 3),  # 3*4 bytes
		("fCameraFront", ctypes.c_float * 3),	 # 3*4 bytes
		("fCameraTop", ctypes.c_float * 3),	   # 3*4 bytes
		("identity", ctypes.c_wchar * 256),	   # 512 bytes
		("context_len", ctypes.c_uint32),		 # 4 bytes
		# ("context", ctypes.c_byte * 256),	   # 256 bytes, see below
		# ("description", ctypes.c_byte * 2048) # 2048 bytes, always empty
	]


class Context(ctypes.Structure):
	_fields_ = [
		("serverAddress", ctypes.c_byte * 28),	# 28 bytes
		("mapId", ctypes.c_uint32),			   # 4 bytes
		("mapType", ctypes.c_uint32),			 # 4 bytes
		("shardId", ctypes.c_uint32),			 # 4 bytes
		("instance", ctypes.c_uint32),			# 4 bytes
		("buildId", ctypes.c_uint32),			 # 4 bytes
		("uiState", ctypes.c_uint32),			 # 4 bytes
		("compassWidth", ctypes.c_uint16),		# 2 bytes
		("compassHeight", ctypes.c_uint16),	   # 2 bytes
		("compassRotation", ctypes.c_float),	  # 4 bytes
		("playerX", ctypes.c_float),			  # 4 bytes
		("playerY", ctypes.c_float),			  # 4 bytes
		("mapCenterX", ctypes.c_float),		   # 4 bytes
		("mapCenterY", ctypes.c_float),		   # 4 bytes
		("mapScale", ctypes.c_float),			 # 4 bytes
		("processID", ctypes.c_uint32),
		("mountIndex", ctypes.c_uint8)
	]


class MumbleLink:

	def __init__(self):
		self.size_link = ctypes.sizeof(Link)
		self.size_context = ctypes.sizeof(Context)
		print("Size link {} context {} wchar {} float {}".format(self.size_link, self.size_context, ctypes.sizeof(ctypes.c_wchar), ctypes.sizeof(ctypes.c_float)))

		self.memfile = mmap.mmap(0, self.size_link + self.size_context, "MumbleLink", mmap.ACCESS_READ)
		self.memfile.seek(0)
		print("Memfile is closed? {}".format(self.memfile.closed))
		obj = ctypes.py_object(self.memfile)
		address = ctypes.c_void_p()
		length = ctypes.c_ssize_t()
		ctypes.pythonapi.PyObject_AsReadBuffer(obj, ctypes.byref(address), ctypes.byref(length))
		int_pointer = address.value
		print("Opened memfile at location {}".format(address))
	
	def get_data_raw(self):
		self.memfile.seek(0)
		data_raw = self.memfile.read(self.size_link)
		context_raw = self.memfile.read(self.size_context)
		return (data_raw, context_raw)
	
	def get_data(self):
		data_raw, context_raw = self.get_data_raw()
		data = self.unpack(Link, data_raw)
		context = self.unpack(Context, context_raw)
		return (data, context)

	def close(self):
		self.memfile.close()

	@staticmethod
	def unpack(ctype, buf):
		cstring = ctypes.create_string_buffer(buf)
		ctype_instance = ctypes.cast(ctypes.pointer(cstring), ctypes.POINTER(ctype)).contents
		return ctype_instance

def main():
	ml = MumbleLink()
	
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	while True:
		d_raw, c_raw = ml.get_data_raw()
		#d = ml.unpack(Link, d_raw)
		#print("Client {} {} {} cam pos {} {} {} cam front {} {} {} | {} ".format(d.fAvatarPosition[0], d.fAvatarPosition[1], d.fAvatarPosition[2], d.fCameraFront[0], d.fCameraFront[1], d.fCameraFront[2], d.fCameraPosition[0], d.fCameraPosition[1], d.fCameraPosition[2], d.name))
		#print("Contextlen {}".format(d.context_len))
		#print(d_raw)
		#print("len of packet d {} complete {}".format(len(d_raw), len(d_raw + c_raw)))
		sock.sendto(d_raw + c_raw, ("127.0.0.1", 7070))
		time.sleep(1/60.0)

	ml.close()


if __name__ == "__main__":
	main()
