import socket	#for sockets
import sys	#for exit
from tkinter import *
import tkinter


class ClientInterface:
	def __init__(self):


		# Variables that are assiged values after server checks #######

		loginFailed = False ## Used only for testing purposes! This variable should be set to True or False depending on the Server Checking of the user credentials!

		############################################################

		## Initialize Inerface

		self.interface = Tk()
		self.interface.title("<<< ChatApp Login >>>")
		self.interface.configure(background="black")
		self.interface.geometry("500x500")
		for i in range(0,3):
			self.interface.grid_columnconfigure(i, weight=1)
		for j in range(0,3):
			self.interface.grid_rowconfigure(j, weight=1)

		## Create Frame 

		self.frame = Frame(self.interface)
		self.frame.configure(background="black")
		self.frame.grid(row=1, column=1)
		
		# Functions ###

		def exitApp(): ## Application Close Function
			#self.interface.destroy() ########### Commented for testing purposes, DO NOT DELETE!
			#exit()					  ########### Commented for testing purposes, DO NOT DELETE!
			self.frame.destroy()
			self.failMessage.destroy()

		def sendToChat(): ## Function that erases Login Interface and initializes the Chat Interface

			# Setting up new interface parameters
			self.interface.resizable(0,0)
			self.interface.geometry("720x720")
			self.listsframe = Frame(self.interface)
			self.listsframe.configure(background="black")
			for i in range(0,3):
				self.listsframe.grid_columnconfigure(i, weight=1)
			for j in range(0,3):
				self.listsframe.grid_rowconfigure(j, weight=1)
			self.listsframe.grid(row=1, column=2)

			# Message History Text box
			self.messageHistory = Text(self.interface, width=40, height=25, wrap=WORD, background="white" ) # TODO add function to save each message that is sent by client and received from the other client!
			self.messageHistory.grid(row=1, column=0)

			# List of Connected Users # TODO Function to get currently connected users or all users
			self.connectedUsersListLabel = Label(self.listsframe, text="Currently Connected Users", bg="black", fg="white", font="none 8 bold", wraplength=65) .grid(row=0, column=0)
			self.cUsersScroll = Scrollbar(self.listsframe)
			self.connectedUsers = Listbox(self.listsframe,yscrollcommand=self.cUsersScroll, width= 15, height=15)
			self.connectedUsers.insert(END, "Gigel")
			self.connectedUsers.grid(row=1, column=0)
			
			# List of Friends # TODO Function to get friends + Add Friend (Probs will use button) + Remove Friend (Probs will use button)
			self.friendsListLabel = Label(self.listsframe, text="Friends list", bg="black", fg="white", font="none 8 bold", wraplength=65) .grid(row=0, column=2)
			self.friendsListScroll = Scrollbar(self.listsframe)
			self.friendsList = Listbox(self.listsframe, yscrollcommand=self.friendsListScroll, width= 15, height=15)
			self.friendsList.insert(END, "Gicu")
			self.friendsList.grid(row=1, column=2)

			# Add Friend / Block User Buttons -> inside listframe!
			self.addFriendButton = Button(self.listsframe, width=10) # TODO add function!
			self.addFriendButton.grid(row=2, column=2)

			self.blockUserButton = Button(self.listsframe, width=10) # TODO add function!
			self.blockUserButton.grid(row=2, column=0)

			# Box where client can input message # TODO Make function that sends the message to the server
			self.messageEntryBox = Entry(self.interface, width=40)
			self.messageEntryBox.grid(row=2, column=0, columnspan=1)

			# Button Frame creation
			self.buttonsFrame = Frame(self.interface)
			self.buttonsFrame.configure(background="black")
			for i in range(0,3):
				self.buttonsFrame.grid_columnconfigure(i, weight=1)
			for j in range(0,3):
				self.buttonsFrame.grid_rowconfigure(j, weight=1)
			self.buttonsFrame.grid(row=2, column=1)

			# Submit written message button (TODO Make it possible to send the message with "Enter")
			self.submitButton = Button(self.buttonsFrame, width=6, height=1) # TODO Add Function Command!
			self.submitButton.grid(row=1, column=2)

			# Add Images Button
			self.addImage = Button(self.buttonsFrame, width=1, height=1) # TODO Add Funtion Command!
			self.addImage.grid(row=1, column=0)

		# Login check function (TODO, send info that the user provided to the server!)
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