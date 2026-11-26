// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
typedef int cpu_type_t;
typedef int cpu_subtype_t;
#define CPU_ARCH_ABI64 0x01000000
#define CPU_TYPE_X86 7
#define CPU_TYPE_X86_64 (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_TYPE_ARM 12
#define CPU_TYPE_ARM64 (CPU_TYPE_ARM | CPU_ARCH_ABI64)
#define CPU_TYPE_POWERPC 18
typedef unsigned long long mach_vm_address_t;
typedef unsigned long long mach_vm_size_t;
typedef int kern_return_t;
typedef unsigned int mach_port_t;
typedef unsigned int mach_port_name_t;
typedef int mach_msg_return_t;
typedef unsigned int boolean_t;
typedef unsigned int mach_msg_bits_t;
typedef unsigned int mach_msg_size_t;
typedef int mach_msg_id_t;
typedef struct { mach_msg_bits_t msgh_bits; mach_msg_size_t msgh_size; mach_port_t msgh_remote_port; mach_port_t msgh_local_port; mach_port_name_t msgh_voucher_port; mach_msg_id_t msgh_id; } mach_msg_header_t;
struct mach_timebase_info { unsigned int numer; unsigned int denom; };
typedef struct mach_timebase_info *mach_timebase_info_t;
kern_return_t mach_timebase_info(mach_timebase_info_t info);
typedef unsigned int vm_map_t;
extern unsigned long vm_page_size;
const char *mach_error_string(kern_return_t error_value);
#define KERN_SUCCESS 0
#define KERN_INVALID_ADDRESS 1
#define KERN_PROTECTION_FAILURE 2
#define KERN_NO_SPACE 3
#define KERN_INVALID_ARGUMENT 4
#define KERN_FAILURE 5
#define KERN_RESOURCE_SHORTAGE 6
#define MACH_PORT_NULL 0
#define MACH_PORT_DEAD ((mach_port_t)~0)
#define MACH_PORT_RIGHT_SEND 0
#define MACH_PORT_RIGHT_RECEIVE 1
#define MACH_PORT_RIGHT_SEND_ONCE 2
#define MACH_PORT_RIGHT_PORT_SET 3
#define MACH_PORT_RIGHT_DEAD_NAME 4
typedef unsigned int mach_port_right_t;
typedef unsigned int mach_msg_type_name_t;
typedef int mach_port_delta_t;
typedef unsigned long long mach_port_context_t;
typedef unsigned int mach_msg_type_number_t;
typedef struct { unsigned int flags; struct { unsigned int mpl_qlimit; } mpl; unsigned int reserved[2]; } mach_port_options_t;
typedef mach_port_t task_t;
typedef mach_port_t task_inspect_t;
extern mach_port_t mach_task_self_;
#define mach_task_self() mach_task_self_
#define MACH_MSG_TYPE_MAKE_SEND 20
#define MACH_MSG_TYPE_COPY_SEND 19
#define MACH_MSG_TYPE_MOVE_SEND 17
#define MACH_MSGH_BITS(remote, local) ((remote) | ((local) << 8))
#define MACH_SEND_MSG 0x1
#define MACH_RCV_MSG 0x2
#define MACH_SEND_TIMEOUT 0x10
#define MACH_RCV_TIMEOUT 0x100
#define MACH_MSG_SUCCESS 0
#define MACH_SEND_TIMED_OUT 0x10000004
#define MACH_RCV_TIMED_OUT 0x10004003
#define MACH_MSG_TIMEOUT_NONE 0
typedef unsigned int mach_msg_option_t;
typedef unsigned int mach_msg_timeout_t;
kern_return_t mach_msg(mach_msg_header_t *msg, mach_msg_option_t option, mach_msg_size_t send_size, mach_msg_size_t rcv_size, mach_port_name_t rcv_name, mach_msg_timeout_t timeout, mach_port_name_t notify);
kern_return_t mach_port_allocate(task_t task, mach_port_right_t right, mach_port_name_t *name);
kern_return_t mach_port_deallocate(task_t task, mach_port_name_t name);
kern_return_t mach_port_insert_right(task_t task, mach_port_name_t name, mach_port_t poly, mach_msg_type_name_t polyPoly);
kern_return_t mach_port_mod_refs(task_t task, mach_port_name_t name, mach_port_right_t right, mach_port_delta_t delta);
kern_return_t mach_port_construct(task_t task, mach_port_options_t *options, mach_port_context_t context, mach_port_name_t *name);
kern_return_t mach_port_destruct(task_t task, mach_port_name_t name, mach_port_delta_t srdelta, mach_port_context_t guard);
kern_return_t mach_port_get_attributes(task_t task, mach_port_name_t name, int flavor, int *info, mach_msg_type_number_t *count);
typedef int vm_prot_t;
typedef int vm_region_flavor_t;
typedef int *vm_region_info_t;
typedef struct { vm_prot_t protection; vm_prot_t max_protection; int inheritance; unsigned int shared; unsigned int reserved; unsigned long long offset; int behavior; unsigned short user_wired_count; } vm_region_basic_info_data_64_t;
#define VM_REGION_BASIC_INFO_64 9
#define VM_PROT_READ 1
#define VM_PROT_WRITE 2
#define VM_PROT_EXECUTE 4
#define VM_REGION_BASIC_INFO_COUNT_64 9
#define MPO_CONTEXT_AS_GUARD 0x01
#define MPO_QLIMIT 0x02
#define MPO_TEMPOWNER 0x04
#define MPO_IMPORTANCE_RECEIVER 0x08
#define MPO_INSERT_SEND_RIGHT 0x10
#define MPO_STRICT 0x20
#define MACH_RCV_LARGE 0x4
#define MACH_RCV_VOUCHER 0x800
#define MACH_RCV_TRAILER_AV 0x20000000
#define MACH_MSG_TRAILER_FORMAT_0 0
#define MACH_RCV_HEADER_ERROR 0x1000000d
#define MACH_RCV_BODY_ERROR 0x1000000c
#define MACH_RCV_TOO_LARGE 0x10004004
#define MACH_MSG_MASK 0x00003e00
#define MACH_MSG_IPC_SPACE 0x00002000
typedef struct voucher_mach_msg_state_s *voucher_mach_msg_state_t;
#define VOUCHER_MACH_MSG_STATE_UNCHANGED ((voucher_mach_msg_state_t)~0)
voucher_mach_msg_state_t voucher_mach_msg_adopt(mach_msg_header_t *msg);
void voucher_mach_msg_revert(voucher_mach_msg_state_t state);
#include <mach/mk_timer.h>
