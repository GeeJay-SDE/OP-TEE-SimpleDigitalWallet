#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <digital_wallet_ta.h>
#include <stdlib.h>
#include <string.h>



int balance;
TransactionList transactions;
// used to track number of transactions. 
int num_transactions;


TEE_Result TA_CreateEntryPoint(void) {
	/* Allocate some resources, init something, etc... */

	DMSG("has been called in digital wallet TA");
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {

	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Hello World!\n");
	init_list(&transactions);
	init_balance(&balance);
	init_num_transactions(&num_transactions);
	IMSG("Initialize balance to: %u", balance);

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}


/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Freeing TransactionList\n");
	free_list(&transactions);
	IMSG("Goodbye!\n");
}

static TEE_Result deposit(uint32_t param_types,
	TEE_Param params[4]) {
	DMSG("has been called");
	IMSG("Got amount to deposit: %u from NW", params[0].value.a);
	balance += params[0].value.a;
	// log the transaction on transaction list
	add_transaction(&transactions, params[0].value.a, DEPOSIT, balance, num_transactions);
	num_transactions += 1;
	params[0].value.a = balance;
	return TEE_SUCCESS;

}

static TEE_Result pay(uint32_t param_types,
	TEE_Param params[4]) {
	DMSG("has been called");
	IMSG("Got amount to pay: %u from NW", params[0].value.a);

	if (balance<params[0].value.a) {
		EMSG("NOT ENOUGH MONEY TO MAKE PAYMENT");
		return TEE_ERROR_GENERIC;
	}
	balance -=  params[0].value.a;
	// log the transaction on transaction list
	add_transaction(&transactions, params[0].value.a, PAYMENT, balance, num_transactions);
	num_transactions += 1;
	params[0].value.a = balance;
	return TEE_SUCCESS;

}

static TEE_Result view_balance(uint32_t param_types,
	TEE_Param params[4]) {

	DMSG("has been called");
	IMSG("Current Balance is: %u", balance);
	params[0].value.a = balance;
	return TEE_SUCCESS;


}

static TEE_Result check_transaction(uint32_t param_types,
	TEE_Param params[4]) {
	DMSG("has been called");

	// Assuming the buffer in params[1].memref is pre-allocated by the normal world
    char* transaction_string = params[0].memref.buffer;
    size_t buffer_size = params[0].memref.size;

    DMSG("obtained buffer and size");

    if (buffer_size < 100) {
    	snprintf(transaction_string, buffer_size, "Buffer is too small");
    	return TEE_ERROR_BAD_PARAMETERS;
    }

    IMSG("Current buffer size provided is: %zu", buffer_size);


    // Clear the buffer at the start (optional, to ensure it is empty)
    transaction_string[0] = '\0';

	// char* transaction_string = get_transactions(&transactions);

	Transaction* current = transactions.head;

	    // If no transactions, set a default message
    if (current == NULL) {
        snprintf(transaction_string, buffer_size, "There are currently no transactions to display.");
        return TEE_SUCCESS;
    }

    int offset = 0; /// keep track of buffer position
    int count = 0;
    while (current != NULL && count < 3) {
    	IMSG("Currently at transaction: %d with amount %d and balance %d", current->transaction_number, current->amount, current->remaining_amount);
    	int written;
    	if (current->transaction_type == PAYMENT){
    		IMSG("Writing payment");
            written = snprintf(transaction_string + offset, buffer_size - offset,
            	"--------------------------------------\n"
                "Transaction ID: %d \n" 
                "Payment made in the amount of %d\n"
                "Reamining Balance after payment is: %d\n"
                "--------------------------------------\n", current->transaction_number, current->amount, current->remaining_amount);
        }
        else {
        	IMSG("Writing Deposit");
        	written = snprintf(transaction_string + offset, buffer_size - offset,
        		"--------------------------------------\n"
                "Transaction ID: %d \n" 
                "Deposit made in the amount of %d\n"
                "Reamining Balance after deposit is: %d\n"
                "---------------------------------------\n", current->transaction_number, current->amount, current->remaining_amount);
        }

        IMSG("Write Success!");

        if (written < 0) {
            // Handle snprintf failure

            snprintf(transaction_string + offset, buffer_size - offset, "Error formatting transaction.");
            return TEE_ERROR_GENERIC;
        }
        
        offset += written;
        IMSG("Offset is now: %d", offset);
        
        if (offset >= buffer_size) {
            // If the buffer is full, stop processing
            break;
        }
        count++;
        current = current->previous_transaction;
    }

        return TEE_SUCCESS;

}

// Take the address of list and dereference it to get the actual TransactionList object
void init_list(TransactionList* list) {
    list->head = NULL;
}

void init_balance(int* balance) {
	// dereference the pointer and assign the value
    *balance = 1000;
}

void init_num_transactions(int* num_transactions) {
	// dereference the pointer and assign the value
	*num_transactions = 0;
}

void add_transaction(TransactionList* transactions, int amount, TRANSACTION_TYPE transaction_type, int remaining_balance, int transaction_number) {

	// address of transaction object
	Transaction* transaction = (Transaction*)malloc(sizeof(Transaction));
	if (!transaction) {
        EMSG("malloc failed!");
        return;
    }

    transaction->amount = amount;
    transaction->transaction_type = transaction_type;
    transaction->remaining_amount = remaining_balance;
    transaction->transaction_number = transaction_number;
    transaction->previous_transaction = NULL;

    if (transactions->head == NULL) {
        transactions->head = transaction;
    }
    else {
    	Transaction* current_node = transactions->head;
    	transaction->previous_transaction = current_node;
    	transactions->head = transaction;
    }
}

//Function to free all nodes in the list
void free_list(TransactionList* transactions) {
    Transaction* current = transactions-> head; //dereferences transactions and gets the head value (which is a pointer)
    Transaction* next_transaction;
    while (current != NULL) {
        next_transaction = current->previous_transaction;
        free(current);
        current = next_transaction;
    }
    transactions->head = NULL;
}


/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_DIGITAL_WALLET_DEPOSIT:
		return deposit(param_types, params);
	case TA_DIGITAL_WALLET_PAY:
		return pay(param_types, params);
	case TA_DIGITAL_WALLET_VIEW_BALANCE:
		return view_balance(param_types, params);
	case TA_DIGITAL_WALLET_CHECK_TRANSACTION:
		return check_transaction(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
