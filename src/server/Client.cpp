/*
 * This file is part of the Doodle3D project (http://doodle3d.com).
 *
 * Copyright (c) 2013, Doodle3D
 * This software is licensed under the terms of the GNU GPL v2 or later.
 * See file LICENSE.txt or visit http://www.gnu.org/licenses/gpl.html for full license details.
 */

#include <stdlib.h>
#include <sys/socket.h>
#include "Client.h"
#include "Logger.h"
#include "Server.h"
#include "CommandHandler.h"
#include "../ipc_shared.h"
#include "../utils.h"

Client::Client(Server& server, int fd)
: logger_(Logger::getInstance()), server_(server), fd_(fd), buffer_(0), bufferSize_(0)
{ /* empty */ }

//TODO: find a way to use recv with extra arg in readAndAppendAvailableData()
int Client::readData() {
	return readAndAppendAvailableData(fd_, &buffer_, &bufferSize_, 0, 1);
}

void Client::runCommands() {
	while (ipc_cmd_is_complete(buffer_, bufferSize_)) {
		CommandHandler::runCommand(*this, buffer_, bufferSize_);
		ipc_cmd_remove(&buffer_, &bufferSize_);
	}
}

bool Client::sendData(const char* buf, int buflen) {
	if (fd_ == -1) return false;

	logger_.logIpcCmd(Logger::BULK, buf, buflen, true); //NOTE: we assume the buffer contains an ipc command

	int rv = ::send(fd_, buf, buflen, 0);

	logger_.checkError(rv, "could not send data in client with fd %i", getFileDescriptor());

	return (rv == buflen);
}


bool Client::sendOk() {
	int cmdlen;
	char* cmd = ipc_construct_cmd(&cmdlen, IPC_CMDR_OK, "");
	if (!cmd) return false;
	sendData(cmd, cmdlen);
	free(cmd);
	return true;
}

bool Client::sendError(const std::string& message) {
	int cmdlen;
	char* cmd = ipc_construct_cmd(&cmdlen, IPC_CMDR_ERROR, "x", message.c_str(), message.length());
	if (!cmd) return false;
	sendData(cmd, cmdlen);
	free(cmd);
	return true;
}


int Client::getFileDescriptor() const {
	return fd_;
}

const char* Client::getBuffer() const {
	return buffer_;
}

int Client::getBufferSize() const {
	return bufferSize_;
}

Server& Client::getServer() {
	return server_;
}

const Server& Client::getServer() const {
	return server_;
}
