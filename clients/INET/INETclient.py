import socket	#for sockets
import sys	#for exit
from tkinter import *
import tkinter
import tkinter.scrolledtext as st
from PIL import Image



class ClientInterface:
	def __init__(self):


		## Initialize Inerface

		self.interface = Tk()
		self.interface.title("<<< ChatApp Login >>>")
		self.interface.configure(background="black")
		self.interface.geometry("500x500")

		# Setting up the Interface Grid
		for i in range(0,3):
			self.interface.grid_columnconfigure(i, weight=1)
		for j in range(0,3):
			self.interface.grid_rowconfigure(j, weight=1)

		## Create Frame 

		self.frame = Frame(self.interface)
		self.frame.configure(background="black")
		self.frame.grid(row=1, column=1)

		# Variables that are assiged values after server checks #######

		self.loginFailed = False ## Used only for testing purposes! This variable should be set to True or False depending on the Server Checking of the user credentials!
		self.currentUser = StringVar()
		self.currentUser.set('Mile')
		
		self.SelectedUser = StringVar()
		self.SelectedUser.set('')
		self.otherUser = StringVar()
		self.otherUser.set('Nobody')
		self.messageToBeSent=StringVar()
		self.messageToBeSent.set('')

		############################################################
		
		# Functions ###

		# Login check function (TODO, send info that the user provided to the server!)
		def loginCheck():
			if(self.loginFailed == True):
				self.failMessage.grid(row=2, column=1, sticky="nsew")
			else:
				self.failMessage.grid_remove()
				self.frame.grid_remove()
				sendToChat()

		def goToRegister():
			self.failMessage.grid_remove()
			self.frame.grid_remove()
			showRegister()

		def exitApp(): ## Application Close Function
			#self.interface.destroy() ########### Commented for testing purposes, DO NOT DELETE!
			#exit()					  ########### Commented for testing purposes, DO NOT DELETE!
			self.frame.destroy()
			self.failMessage.destroy()

		def AddFriend():
			check = self.connectedUsers.selection_get()
			fullList= list(self.friendsList.get(0,END))
			if check not in fullList:
				userToBeAdded=check
				print(userToBeAdded)
				self.friendsList.insert(END, userToBeAdded)
			#TODO Add verification / send info to server / DB

		def RemoveFriend():
			check = self.connectedUsers.selection_get()
			fullList= list(self.friendsList.get(0,END))
			if check in fullList:
				userToBeRemoved=check
				print(userToBeRemoved)
				self.friendsList.delete(self.friendsList.get(0,END).index(userToBeRemoved))
			#TODO Add verification / send info to server / DB

		def ChatWith(): 
			check = self.connectedUsers.selection_get()
			fullList= list(self.friendsList.get(0,END))
			print(check)
			if check in fullList:
				self.otherUser.set(str(check))
			else:
				self.otherUser.set('Nobody')
			#TODO Add verification / send info to server / DB

		def SendText():
			if(self.otherUser.get() != 'Nobody'):
				textToSend=self.messageEntryBox.get()
				self.messageToBeSent=str(self.currentUser.get())+': '+str(textToSend)+'\n'
				self.messageHistory.configure(state='normal')
				self.messageHistory.insert(END, self.messageToBeSent)
				self.messageHistory.update_idletasks()
				self.messageHistory.configure(state='disabled')
			#TODO apel cu mesaj catre >>> self.otherUser <<<

		def backToLogin():
			self.fieldsFrame.grid_remove()
			self.interface.title("<<< ChatApp Login >>>")
			self.frame.grid(row=1, column=1)


		def submitRegisterInfo():
			print('ceva')

		def AddImage():
			#TODO open file browser and send image as bytes
			print("Hehe")

####################################################### CHAT ######################################################################
		def sendToChat(): ## Function that erases Login Interface and initializes the Chat Interface

			# Setting up new interface parameters
			self.interface.resizable(0,0)
			self.interface.geometry("720x720")
			self.interface.configure(background="gray10")
			self.interface.title("<<< Chat App >>>")

			self.listsframe = Frame(self.interface)
			self.listsframe.configure(background="gray10")
			for i in range(0,3):
				self.listsframe.grid_columnconfigure(i, weight=1)
			for j in range(0,3):
				self.listsframe.grid_rowconfigure(j, weight=1)
			self.listsframe.grid(row=1, column=1)

			# Frame with account change request buttons
			changeRequestButtons = Frame(self.interface)
			for i in range(0,1):
				changeRequestButtons.grid_rowconfigure(i, weight=1)
			for j in range(0,4):
				changeRequestButtons.grid_columnconfigure(j, weight=1)
			changeRequestButtons.grid(row=0, column=0)
			

			Button(changeRequestButtons, text="Change Avatar") .grid(row=0, column=1)
			Button(changeRequestButtons, text="Change Username") .grid(row=0, column=2)
			Button(changeRequestButtons, text="Change Password") .grid(row=0, column=3)


			# User currently talking to
			self.curUserLabel = Label(self.interface, textvariable=self.otherUser, bg="gray10", fg="white", font="none 8 bold") .grid(row=0, column=0, sticky=S)
			self.talkingToLabel = Label(self.interface, text="Currently talking to: ", bg="gray10", fg="white", font="none 8 bold") .grid(row=0, column=0, sticky=SW)

			# Message History Text box
			self.messageHistory = st.ScrolledText(self.interface, width=40, height=25 ) # TODO add function to save each message that is sent by client and received from the other client!
			self.messageHistory.grid(row=1, column=0)

			# List of Connected Users # TODO Function to get currently connected users or all users
			self.connectedUsersListLabel = Label(self.listsframe, text="Currently Connected Users", bg="gray10", fg="white", font="none 8 bold", wraplength=65) .grid(row=0, column=0)
			self.cUsersScroll = Scrollbar(self.listsframe)
			self.connectedUsers = Listbox(self.listsframe,yscrollcommand=self.cUsersScroll.set, width= 15, height=15, selectmode=SINGLE)
			self.cUsersScroll.config(command = self.connectedUsers.yview)
			self.connectedUsers.insert(END, "Gigel")
			self.connectedUsers.insert(END, "Keke")
			self.connectedUsers.insert(END, "Juger")
			self.connectedUsers.grid(row=1, column=0)
			
			# List of Friends # TODO Function to get friends + Add Friend (Probs will use button) + Remove Friend (Probs will use button)
			self.friendsListLabel = Label(self.listsframe, text="Friends list", bg="gray10", fg="white", font="none 8 bold", wraplength=65) .grid(row=0, column=2)
			self.friendsListScroll = Scrollbar(self.listsframe)
			self.friendsList = Listbox(self.listsframe, yscrollcommand=self.friendsListScroll, width= 15, height=15, selectmode=SINGLE)
			self.friendsList.insert(END, "Gicu")
			self.friendsList.insert(END, "Ana")
			self.friendsList.insert(END, "Mihai")
			self.friendsList.grid(row=1, column=2)

			# New Frame for all user action buttons
			self.userActionButtons = Frame(self.listsframe)
			for i in range(0,1):
				self.userActionButtons.grid_columnconfigure(i, weight=1)
			for j in range(0,6):
				self.userActionButtons.grid_rowconfigure(j, weight=1)
			self.userActionButtons.grid(row=1, column=1)

			# Add Friend / Block User Buttons -> inside listframe!
			self.addFriendButton = Button(self.userActionButtons, width=10, text="Add Friend", command=AddFriend) # TODO add function!
			self.addFriendButton.grid(row=0, column=0)

			self.removeFriendButton = Button(self.userActionButtons, width=10, text="Remove Friend", command=RemoveFriend) # TODO add function!
			self.removeFriendButton.grid(row=1, column=0)

			self.chatWithUser = Button(self.userActionButtons, width=10, text="Talk to", command=ChatWith) # TODO add function!
			self.chatWithUser.grid(row=2, column=0)

			# Box where client can input message
			self.messageEntryBox = Entry(self.interface, width=40, textvariable="")
			self.messageEntryBox.grid(row=2, column=0, columnspan=1)

			# Send Message and Send Image Buttons Frame creation
			self.buttonsFrame = Frame(self.interface)
			self.buttonsFrame.configure(background="gray10")
			for i in range(0,3):
				self.buttonsFrame.grid_columnconfigure(i, weight=1)
			for j in range(0,3):
				self.buttonsFrame.grid_rowconfigure(j, weight=1)
			self.buttonsFrame.grid(row=2, column=1)

			# Submit written message button (TODO Make it possible to send the message with "Enter")
			self.submitButton = Button(self.buttonsFrame, width=4, height=1, text="Send", command=SendText) # TODO Add Function Command!
			self.submitButton.grid(row=1, column=2)

			# Add Images Button
			self.addImage = Button(self.buttonsFrame, width=1, height=1, text="Img", command=AddImage) # TODO Add Funtion Command!
			self.addImage.grid(row=1, column=0)

			self.messageHistory.configure(state='disabled')
			# Exit/Logout
			# TODO>>

#################################################################################################################################

################################################### Register Interface ###################################################

		def showRegister():
			self.interface.title("<<< ChatApp Register >>>")

			self.fieldsFrame = Frame(self.interface)
			for i in range(0,5):
				self.fieldsFrame.grid_rowconfigure(i, weight=1)
			for j in range(0,3):
				self.fieldsFrame.grid_columnconfigure(j, weight=1)
			self.fieldsFrame.configure(background="black")
			self.fieldsFrame.grid(row=1, column=1)

			# Username Entry field
			Label(self.fieldsFrame, text="Username: ", background="black", fg="white") .grid(row=0, column=0)
			usernameRegisterEntry = Entry(self.fieldsFrame) .grid(row=0, column=1)

			# Email Entry field
			Label(self.fieldsFrame, text="Email: ",background="black", fg="white") .grid(row=1, column=0)
			emailRegisterEntry = Entry(self.fieldsFrame) .grid(row=1, column=1)

			# Password Entry field
			Label(self.fieldsFrame, text="Password: ",background="black", fg="white") .grid(row=2, column=0)
			passwordRegisterEntry = Entry(self.fieldsFrame) .grid(row=2, column=1)

			# Phone Number Entry field
			Label(self.fieldsFrame, text="Phone Number: ",background="black", fg="white") .grid(row=3, column=0)
			phonenoRegisterEntry = Entry(self.fieldsFrame) .grid(row=3, column=1)
			
			# Error creating account Message
			errorMessage = Label(self.interface, textvariable = "") 
			errorMessage.config(background="black", fg="red")
			errorMessage.grid(row=0, column=1) 

			# Back to login button
			Button(self.fieldsFrame, text="Back to Login", width=10, command=backToLogin) .grid(row=4, column=0, sticky=NW)
			# Submit info button
			Button(self.fieldsFrame, text="Submit", width=6, command=submitRegisterInfo) .grid(row=4, column=1, sticky=NE)

			

###########################################################################################################################


		# Username Field
		Label(self.frame, text="Username:", bg="black", fg="white", font="none 12 bold") .grid(row=0, column=1, sticky="nsew")
		self.usernameEntry = Entry(self.frame, width=20, bg="white")
		self.usernameEntry.grid(row=0, column= 2, sticky="nsew")

		# Password Field
		Label (self.frame, text="Password:", bg="black", fg="white", font="none 12 bold") .grid(row=1, column=1, sticky="nsew")
		self.passwordEntry = Entry(self.frame, width=20, bg="white")
		self.passwordEntry.grid(row=1, column= 2, sticky="nsew")

		# Check Login Info / Login Button
		Button(self.frame, text="Login", width=6, command=loginCheck) .grid(row=2, column=1, sticky="nsew")

		# Exit App Button
		Button(self.frame, text="Exit", width=4, command=exitApp) .grid(row=2, column=2, sticky="nsew")

		# Register Button
		Button(self.frame, text="Register", width=8, command=goToRegister) .grid(row=2, column=0, sticky="nsew")

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