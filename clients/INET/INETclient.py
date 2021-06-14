import socket	#for sockets
import sys	#for exit
import os
from io import BytesIO
from tkinter import *
import tkinter
import tkinter.scrolledtext as st
from tkinter import filedialog
from PIL import ImageTk

userToken='jd30jc0sjh3'
addFriend= 'addfriend'
removeFriend= 'removefriend'
getHistroy= 'getmessageslist'
sendMessage= 'sendmessage'

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

		self.currentUser = StringVar()
		self.currentUser.set('')
		
		self.otherUser = StringVar()
		self.otherUser.set('Nobody')
		self.messageToBeSent=StringVar()
		self.messageToBeSent.set('')
		
		###########################################################################################################
		
		#################################################### Functions ###########################################

		# Login check function
		def loginCheck():
			if(self.usernameLoginEntry.get() != '' or self.passwordLoginEntry.get() != ''):
				usernameLength = len(self.usernameLoginEntry.get())
				passwordLength = len(self.passwordLoginEntry.get())
				credentialsLength = usernameLength+passwordLength+2
				#fullLength = str(credentialsLength+ len(str(credentialsLength))+ 1)
				messageToSend = str(credentialsLength) + ':' + ';' + self.usernameLoginEntry.get() + ',' + self.passwordLoginEntry.get()

				# TODO send info to server + wait for a true/false statement.
				
				loginFailed = ClientConnection.__init__.sendMessageToServer(self, messageToSend)
				if(loginFailed == False):
					receivedMessage=''
					while receivedMessage == '':
						receivedMessage = ClientConnection.__init__.listenToMessages(self)

					if(receivedMessage == ''):
						self.failMessage.grid(row=2, column=1, sticky="nsew")
					else:
						userToken = receivedMessage
						self.currentUser.set(self.usernameLoginEntry.get())
						self.failMessage.grid_remove()
						self.frame.grid_remove()	
						sendToChat()
				else:
					print('Failed to send message')

		def goToRegister():
			self.failMessage.grid_remove()
			self.frame.grid_remove()
			showRegister()

		## Application Close Function
		def exitApp(): 
			self.interface.destroy()
			exit()					  
		
		def AddFriend():
			check = self.currentUsersList.selection_get()
			fullList= list(self.friendsList.get(0,END))
			if check not in fullList:
				userToBeAdded=check
				credentialsLength= str(len(self.currentUser.get()) + len(str(userToBeAdded)) + len(userToken)+ len(addFriend)+ 3)
				messageToSend = credentialsLength + ':' + addFriend + ';' + self.currentUser.get() + ',' + userToBeAdded + ',' + userToken
				ClientConnection.__init__.sendMessageToServer(self, messageToSend)

		def RemoveFriend():
			check = self.currentUsersList.selection_get()
			fullList= list(self.friendsList.get(0,END))
			if check in fullList:
				userToBeRemoved=check
				credentialsLength= str(len(self.currentUser.get()) + len(str(userToBeRemoved)) + len(userToken)+ len(removeFriend)+ 3)
				messageToSend = credentialsLength + ':' + removeFriend + ';' + self.currentUser.get() + ',' + userToBeRemoved + ',' + userToken
				ClientConnection.__init__.sendMessageToServer(self, messageToSend)

		def ChatWith(): 
			check = self.currentUsersList.selection_get()
			fullList= list(self.friendsList.get(0,END))
			if check in fullList:
				self.otherUser.set(str(check))
				credentialsLength = str(len(self.currentUser.get())+ len(self.otherUser.get())+ len(userToken) + len(getHistroy)+ 3)
				messageToSend = credentialsLength + ':' + getHistroy + ';' + self.currentUser.get() + ',' + self.otherUser.get() + ',' + userToken
				ClientConnection.__init__.sendMessageToServer(self, messageToSend)
				returnValue=''
				# TODO Make with threads
				returnValue = ClientConnection.__init__.listenToMessages(self)
				if(returnValue != ''):
					self.messageHistory.configure(state='nomral')
					self.messageHistory.insert(END, returnValue)
					self.messageHistory.update_idletasks()
					self.messageHistory.configure(state='disabled')
			else:
				self.otherUser.set('Nobody')

		def SendText():
			if(self.otherUser.get() != 'Nobody' and self.messageEntryBox.get() != ''):
				textToSend=self.messageEntryBox.get()

				credentialsLength = str(len(sendMessage) + len(self.currentUser.get()) + len(self.otherUser.get()) + len(userToken) + len(textToSend) + 4)
				messageToSend = credentialsLength + ':' + sendMessage + ';' + self.currentUser.get() + ',' + self.otherUser.get() + ',' + userToken + ',' + textToSend
				
				returnedValue = ClientConnection.__init__.sendMessageToServer(self, messageToSend)
				
				self.messageToBeSent=str(self.currentUser.get())+': '+str(textToSend)+'\n'
				self.messageHistory.configure(state='normal')
				self.messageHistory.insert(END, self.messageToBeSent)
				self.messageHistory.update_idletasks()
				self.messageHistory.configure(state='disabled')
				self.messageEntryBox.delete(0, END)
			

		def backToLogin():
			self.fieldsFrame.grid_remove()
			self.interface.title("<<< ChatApp Login >>>")
			self.frame.grid(row=1, column=1)


		def submitRegisterInfo():
			print('ceva')

		def AddFile():
			rep = filedialog.askopenfilenames(parent=self.interface, initialdir='~/Desktop', initialfile='', filetypes=[("All files", "*")])
			print(rep)
			file_stream = BytesIO()
			counter=0
			with open(rep[0], 'rb') as file:
				byte = file.read(1)
				while byte:
					file_stream.write(byte)
					byte= file.read(1)
					counter=counter+1
			file_stream.seek(0)
			
			with open('plm', "wb") as file:
				byte = file_stream.read(1)
				while byte:
					file.write(byte)
					byte= file_stream.read(1)
					

			print(counter)
			#TODO open file browser and send image as bytes

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

			Button(changeRequestButtons, text="Change Email") .grid(row=0, column=2)

			Button(changeRequestButtons, text="Change Password") .grid(row=0, column=3)


			# User currently talking to
			self.curUserLabel = Label(self.interface, textvariable=self.otherUser, bg="gray10", fg="white", font="none 8 bold") .grid(row=0, column=0, sticky=S)
			self.talkingToLabel = Label(self.interface, text="Currently talking to: ", bg="gray10", fg="white", font="none 8 bold") .grid(row=0, column=0, sticky=SW)

			# Message History Text box
			self.messageHistory = st.ScrolledText(self.interface, width=40, height=25 )
			self.messageHistory.grid(row=1, column=0)

			# List of Connected Users # TODO Function to get currently connected users or all users
			self.currentUsersListLabel = Label(self.listsframe, text="Current Users", bg="gray10", fg="white", font="none 8 bold", wraplength=65) .grid(row=0, column=0)
			self.cUsersScroll = Scrollbar(self.listsframe)
			self.currentUsersList = Listbox(self.listsframe,yscrollcommand=self.cUsersScroll.set, width= 15, height=15, selectmode=SINGLE)
			self.cUsersScroll.config(command = self.currentUsersList.yview)
			self.currentUsersList.insert(END, "Gigel")
			self.currentUsersList.insert(END, "Keke")
			self.currentUsersList.insert(END, "Juger")
			self.currentUsersList.grid(row=1, column=0)
			
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
			self.addImage = Button(self.buttonsFrame, width=5, height=1, text="Add File", command=AddFile) # TODO Add Funtion Command!
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

####################################### Login Interface ############################################################

		# Username Field
		Label(self.frame, text="Username:", bg="black", fg="white", font="none 12 bold") .grid(row=0, column=1, sticky="nsew")
		self.usernameLoginEntry = Entry(self.frame, width=20, bg="white")
		self.usernameLoginEntry.grid(row=0, column= 2, sticky="nsew")

		# Password Field
		Label (self.frame, text="Password:", bg="black", fg="white", font="none 12 bold") .grid(row=1, column=1, sticky="nsew")
		self.passwordLoginEntry = Entry(self.frame, width=20, bg="white", show='*')
		self.passwordLoginEntry.grid(row=1, column= 2, sticky="nsew")

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

##########################################################################################################################

################################################# Connection #############################################################
class ClientConnection:

	def __init__(self):
		#create an INET, STREAMing socket
		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		except socket.error:
			print('Failed to create socket')
			sys.exit()
			
		print('Socket Created')

		host = 'localhost'
		port = 6000             
		
		#Connect to remote server
		#s.connect((host , port))

		print('Socket Connected to ' + host + ' on ip ' + host + ':' + str(port))

		#Now receive data
		def listenToMessages(self):
			try:
				reply = s.recv(4096)
			except socket.error:
				return("Failed to receive message")

			return(reply)

		#Send some data to remote server
		def sendMessageToServer(self,message):
			try :
				#Set the whole string
				s.sendall(message)
			except socket.error:
				#Send failed
				return(True)
				
			return(False)


		

		
		
		#s.close()
	#############################################################################################

	

Conn = ClientConnection()
GUI = ClientInterface()
