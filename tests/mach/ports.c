// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Mach ports through the broker of the session: messages to a port of the same process, a reply through a send-once right,
// bootstrap names between two processes, a port in a message, and what is left after a process ends.  Port names are not
// printed: they hold process numbers.
#include "darwin.h"

struct simple { mach_msg_header_t header; char text[32]; };
struct received { mach_msg_header_t header; char text[32]; char trailer[64]; };
struct carrying { mach_msg_header_t header; uint32_t count; mach_msg_port_descriptor_t port; char text[16]; };
struct carried { mach_msg_header_t header; uint32_t count; mach_msg_port_descriptor_t port; char text[16]; char trailer[64]; };

static kern_return_t sendText(mach_port_t to, uint32_t bits, mach_port_t reply, int id, const char* text)
{
    struct simple message;
    memset(&message, 0, sizeof message);
    message.header.msgh_bits = bits;
    message.header.msgh_size = sizeof message;
    message.header.msgh_remote_port = to;
    message.header.msgh_local_port = reply;
    message.header.msgh_id = id;
    snprintf(message.text, sizeof message.text, "%s", text);
    return mach_msg(&message, MACH_SEND_MSG | MACH_SEND_TIMEOUT, sizeof message, 0, 0, 1000, 0);
}

static kern_return_t receiveText(mach_port_t on, struct received* message, uint32_t timeout)
{
    memset(message, 0, sizeof *message);
    return mach_msg(message, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0, sizeof *message, on, timeout, 0);
}

int main(void)
{
    mach_port_t port = 0, reply = 0;
    struct received in;
    printf("allocate %d\n", mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port));
    printf("insert %d\n", mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND));
    printf("send %d\n", sendText(port, MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0), 0, 100, "hello"));
    kern_return_t result = receiveText(port, &in, 1000);
    printf("receive %d: id %d, size %u, text %s, reply %d\n", result, in.header.msgh_id, in.header.msgh_size, in.text, in.header.msgh_local_port != 0);
    printf("empty %#x\n", receiveText(port, &in, 50));

    // A request with a send-once right for the answer.
    printf("allocate reply %d\n", mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &reply));
    printf("request %d\n", sendText(port, MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE), reply, 200, "question"));
    result = receiveText(port, &in, 1000);
    printf("got request %d: id %d, text %s, reply %d\n", result, in.header.msgh_id, in.text, in.header.msgh_local_port != 0);
    printf("answer %d\n", sendText(in.header.msgh_local_port, MACH_MSGH_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE, 0), 0, 201, "answer"));
    result = receiveText(reply, &in, 1000);
    printf("got answer %d: id %d, text %s\n", result, in.header.msgh_id, in.text);

    // Bootstrap names between two processes; the child sends a port of its own and waits on it.
    mach_port_t service = 0;
    printf("check in %d\n", bootstrap_check_in(bootstrap_port, "org.pearos.test.ports", &service));
    mach_port_t unknown = 0;
    printf("look up unknown %d\n", bootstrap_look_up(bootstrap_port, "org.pearos.test.nothing", &unknown));
    fflush(__stdoutp);
    pid_t child = fork();
    if (child == 0) {
        mach_port_t found = 0, own = 0;
        kern_return_t looked = bootstrap_look_up(bootstrap_port, "org.pearos.test.ports", &found);
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &own);
        struct carrying message;
        memset(&message, 0, sizeof message);
        message.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0) | MACH_MSGH_BITS_COMPLEX;
        message.header.msgh_size = sizeof message;
        message.header.msgh_remote_port = found;
        message.header.msgh_id = 300;
        message.count = 1;
        message.port.name = own;
        message.port.disposition = MACH_MSG_TYPE_MAKE_SEND;
        snprintf(message.text, sizeof message.text, "from child");
        kern_return_t sent = mach_msg(&message, MACH_SEND_MSG | MACH_SEND_TIMEOUT, sizeof message, 0, 0, 2000, 0);
        struct received answer;
        kern_return_t got = receiveText(own, &answer, 5000);
        printf("child: look up %d, send %d, answer %d: id %d, text %s\n", looked, sent, got, answer.header.msgh_id, answer.text);
        fflush(__stdoutp);
        _exit(0);
    }
    struct carried request;
    memset(&request, 0, sizeof request);
    result = mach_msg(&request, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0, sizeof request, service, 5000, 0);
    printf("service %d: id %d, complex %d, count %u, disposition %u, text %s\n", result, request.header.msgh_id,
           (request.header.msgh_bits & MACH_MSGH_BITS_COMPLEX) != 0, request.count, request.port.disposition, request.text);
    printf("to child %d\n", sendText(request.port.name, MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0), 0, 301, "from parent"));
    int status = 0;
    waitpid(child, &status, 0);
    fflush(__stdoutp);
    printf("child ended %d\n", status);
    // The broker learns about the end of the child on its own connection: give it the time to tell this process.
    usleep(300000);
    printf("to dead child %#x\n", sendText(request.port.name, MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0), 0, 302, "late"));
    printf("deallocate %d\n", mach_port_deallocate(mach_task_self(), request.port.name));
    return 0;
}
