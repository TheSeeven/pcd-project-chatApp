import socket	#for sockets
import sys	#for exit
from tkinter import *
import tkinter


class ClientInterface:
	def __init__(self):


		# Variables #######

		loginFailed = False

		###############

		self.interface = Tk()
		self.interface.title("<<< ChatApp Login >>>")
		self.interface.configure(background="black")
		self.interface.geometry("500x500")
		
		self.frame = Frame(self.interface)
		self.frame.configure(background="black")
		for i in range(0,3):
			self.interface.grid_columnconfigure(i, weight=1)
		for j in range(0,3):
			self.interface.grid_rowconfigure(j, weight=1)
		self.frame.grid(row=1, column=1)
		
		# Functions ###

		def exitApp():
			#self.interface.destroy() ########### Commented for testing purposes, DO NOT DELETE!
			#exit()					  ########### Commented for testing purposes, DO NOT DELETE!
			self.frame.destroy()
			self.failMessage.destroy()

		def sendToChat():
			# Set new geometry for interface (TODO, make it a fixed size!)
			self.interface.geometry("720x720")
			self.listsframe = Frame(self.interface)
			self.listsframe.configure(background="black")
			self.listsframe.grid(row=0, column=2)

			# Message History Text box
			self.messageHistory = Text(self.interface, width=40, height=20, wrap=WORD, background="white", )
			self.messageHistory.grid(row=0, column=0)

			# List of Connected Users
			self.cUsersScroll = Scrollbar(self.listsframe)
			self.connectedUsers = Listbox(self.listsframe,yscrollcommand=self.cUsersScroll)
			self.connectedUsers.insert(END, "Gigel")
			self.connectedUsers.grid(row=0, column=0)
			
			# List of Friends
			self.friendsListScroll = Scrollbar(self.listsframe)
			self.friendsList = Listbox(self.listsframe, yscrollcommand=self.friendsListScroll)
			self.friendsList.insert(END, "Gicu")
			self.friendsList.grid(row=0, column=1)

			# Box where client can input message
			self.messageEntryBox = Entry(self.interface, width=40)
			self.messageEntryBox.grid(row=2, column=0, columnspan=1)

			# Submit written message button (TODO Make it possible to send the message with "Enter")
			self.submitButton = Button(self.interface, width=10, height=1)
			self.submitButton.grid(row=2, column=1, columnspan= 2)

		# Login check function (TODO, send info that the user provided)
		def loginCheck():
			if(loginFailed == True):
				self.failMessage.grid(row=2, column=1, sticky="nsew")
			else:
				self.failMessage.grid_remove()
				self.frame.grid_remove()
				sendToChat()

		###############

		# Username Field
		Label (self.frame, text="Username:", bg="black", fg="white", font="none 12 bold") .grid(row=2, column=1, sticky="nsew")
		self.usernameEntry = Entry(self.frame, width=20, bg="white")
		self.usernameEntry.grid(row=2, column= 2, sticky="nsew")

		# Password Field
		Label (self.frame, text="Password:", bg="black", fg="white", font="none 12 bold") .grid(row=3, column=1, sticky="nsew")
		self.passwordEntry = Entry(self.frame, width=20, bg="white")
		self.passwordEntry.grid(row=3, column= 2, sticky="nsew")

		# Check Login Info / Login Button
		Button(self.frame, text="Login", width=6, command=loginCheck) .grid(row=4, column=1, sticky="nsew")

		# Exit App Button
		Button(self.frame, text="Exit", width=4, command=exitApp) .grid(row=4, column=2, sticky="nsew")

		# Failed Login Message
		self.failMessage = Label(self.interface, text="Invalid username or password!", width=10)
		self.failMessage.config(bg="black", fg="red")
		self.failMessage.grid(row=2, column=1, sticky="nsew")
		self.failMessage.grid_remove()

		self.interface.mainloop()

GUI = ClientInterface()
"""
class ClientConnection:
	#create an INET, STREAMing socket
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	except socket.error:
		print('Failed to create socket')
		sys.exit()
		
	print('Socket Created')

	host = 'www.google.com' # TODO Change Host + PORT
	port = 80               # $$$$$$$$$$$$$$$$$$$

	try:
		remote_ip = socket.gethostbyname( host )

	except socket.gaierror:
		#could not resolve
		print('Hostname could not be resolved. Exiting')
		sys.exit()

	#Connect to remote server
	s.connect((remote_ip , port))

	print('Socket Connected to ' + host + ' on ip ' + remote_ip)

	#Send some data to remote server
	message = "GET / HTTP/1.1\r\n\r\n"

	try :
		#Set the whole string
		s.sendall(message)
	except socket.error:
		#Send failed
		print('Send failed')
		sys.exit()

	print('Message send successfully')

	#Now receive data
	try:
		reply = s.recv(4096)
	except socket.error:
		print('Failed to receive messages.')
		sys.exit()

	print(reply)

	s.close()
	"""