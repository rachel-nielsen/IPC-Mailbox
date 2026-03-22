#include "mailbox.h"
/*
 * Initialize a mailbox structure.
 * Must be called exactly once before fork().
 */
void mailbox_init(mailbox_t *mbox){
    // Return if mbox is NULL
    if(mbox == NULL){ 
        return;
    }
    // Initialize mbox's head and tail 
    mbox -> head = 0;
    mbox -> tail = 0;
    for(int i = 0; i < MAILBOX_CAPACITY; i++){
        mbox -> slots[i].len = 0; // Initialize mbox slots' lengths
    }
}
/*
 * Asynchronous send.
 *
 * Returns:
 *   MBOX_SUCCESS on success
 *   MBOX_ERR_FULL if mailbox is full
 *   MBOX_ERR_TOO_LARGE if len > MAILBOX_MAX_MSG
 *   MBOX_ERR_INVALID on invalid arguments
 */
int send_mailbox(mailbox_t *mbox, const void *msg, size_t len){ // Parameters: pointer to mailbox to send to, pointer to message to send, length of message to send in bytes
    if(mbox == NULL || msg == NULL || len == 0){ // If parameters are NULL/invalid arguments
        return MBOX_ERR_INVALID;
    }

    if(((mbox -> tail + 1) % MAILBOX_CAPACITY) == (mbox -> head)){ // If mbox's tail is (MAILBOX_CAPACITY - 1)
        return MBOX_ERR_FULL;
    }
    
    if(len > MAILBOX_MAX_MSG){ // If message length is larger than MAILBOX_MAX_MSG
        return MBOX_ERR_TOO_LARGE;
    }

    mbox -> slots[mbox -> tail].len = len; // Access length of slot at mbox's tail sets length
    memcpy(mbox -> slots[mbox -> tail].data, msg, len); // Copy message to mbox's tail's data variable
    mbox -> tail = (mbox -> tail + 1) % MAILBOX_CAPACITY; // Set mbox's tail to next slot, loops to beginning, if next tail is MAILBOX_CAPACITY
    return MBOX_SUCCESS; 
}
/*
 * Asynchronous receive.
 *
 * Copies at most maxlen bytes into out_buf.
 *
 * On success:
 *   - returns MBOX_SUCCESS
 *   - writes actual message length to *out_len
 *
 * Returns:
 *   MBOX_ERR_EMPTY if mailbox is empty
 *   MBOX_ERR_INVALID on invalid arguments
 */
int receive_mailbox(mailbox_t *mbox, void *out_buf, size_t maxlen, size_t *out_len){ // Parameters: mbox to receive from, memory buffer, memory buffer's length, and message length
    if(mbox == NULL || out_buf == NULL || maxlen == 0 ||  out_len == NULL){ // If parameters are NULL/invalid arguments
        return MBOX_ERR_INVALID;
    }

    if(mbox -> head == mbox -> tail){ // If mbox's head is equal to it's tail
        return MBOX_ERR_EMPTY;
    }

    memset(out_buf, 0, maxlen); // Clear buffer

    size_t len = mbox -> slots[mbox -> head].len; // Declare len as length of message mbox's head
    for(size_t i = 0; i < len; i++){ // Loop for either length of message
        ((uint8_t *)out_buf)[i] = mbox -> slots[mbox -> head].data[i]; // Typecast out_buf to bytes, defines byte i of out_buf as byte i of mbox's head's data
    }
    *out_len = len; // Dereference out_len and store length of message

    mbox -> head = (mbox -> head + 1) % MAILBOX_CAPACITY; // Set's mbox's head to next slot, loops to beginning, if next slot is MAILBOX_CAPACITY
    return MBOX_SUCCESS;
}
