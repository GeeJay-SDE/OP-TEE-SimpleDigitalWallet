#include <err.h>
#include <stdio.h>
#include <string.h>

// Needed for Digital Wallet TA UUID
#include <digital_wallet_ta.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>


int main(void){

	printf("Hello, World! from gary's VM \n" );

	// Space is allocated on
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_DIGITAL_WALLET_UUID;
	uint32_t err_origin;

	/* Initialize a context connecting us to the TEE */
	// &ctx stores the address of ctx
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the Digital Wallet Trusted Application, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	/*
	 * Execute a function in the TA by invoking it, in this case
	 * we're incrementing a number.
	 *
	 * The value of command ID part and how the parameters are
	 * interpreted is part of the interface provided by the TA.
	 */

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * Pass a memref in the secondparameter the remaining three parameters are unused.
	 */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	/*
	 * TA_DIGITAL_WALLET_VIEW_BALANCE is the actual function in the TA to be
	 * called. We will be checking the initial balance
	 */
	printf("Checking initial balance...\n");
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_VIEW_BALANCE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	printf("Current balance is: %d\n", op.params[0].value.a);
	
	// check Transactions
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);

	//provide a buffer for writing in secure world to pass to normal world
	char* transaction_buffer[MAX_STRING_SIZE] = {0};
	// Set the buffer in the params
	op.params[0].tmpref.buffer = transaction_buffer;
	op.params[0].tmpref.size = sizeof(transaction_buffer);

	printf("Getting recent transactions...\n");
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_CHECK_TRANSACTION, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

    printf("Here are some recent transactions (Showing 3 max.):\n %s\n", op.params[0].tmpref.buffer);

	// Making a deposit of 57
	printf("Making a deposit of 57...\n");
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 57;
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_DEPOSIT, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("Balance after deposit is: %d\n", op.params[0].value.a);

	// check Transactions
	printf("Getting past transactions...\n");
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);

	//provide a buffer for writing in secure world to pass to normal world
	memset(transaction_buffer, 0, sizeof(transaction_buffer));
	// Set the buffer in the params
	op.params[0].tmpref.buffer = transaction_buffer;
	op.params[0].tmpref.size = sizeof(transaction_buffer);

	printf("Getting past transactions...\n");
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_CHECK_TRANSACTION, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

    printf("Here are some recent transactions (Showing 3 max.):\n %s\n", op.params[0].tmpref.buffer);

	// Making a deposit of 29
    memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 29;
	printf("Making a deposit of 29...\n");
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_DEPOSIT, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	printf("Balance after deposit is: %d\n", op.params[0].value.a);


	// Making a deposit of 83
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 83;
	printf("Making a deposit of 83...\n");
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_DEPOSIT, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	printf("Balance after deposit is: %d\n", op.params[0].value.a);

	// Making a payment of 17
	printf("Making a payment of 17...\n");
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 17;
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_PAY, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("Balance after payment is: %d\n", op.params[0].value.a);

	// check Transactions
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);

	//provide a buffer for writing in secure world to pass to normal world
	memset(transaction_buffer, 0, sizeof(transaction_buffer));
	// Set the buffer in the params
	op.params[0].tmpref.buffer = transaction_buffer;
	op.params[0].tmpref.size = sizeof(transaction_buffer);

	printf("Getting past transactions...\n");
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_CHECK_TRANSACTION, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

    printf("Here are some recent transactions (Showing 3 max.):\n %s\n", op.params[0].tmpref.buffer);

    // Making a payment of 2000 (NOT ENOUGH, should error out)
	// printf("Making a payment of 2000...\n");
	// memset(&op, 0, sizeof(op));
	// op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
	// 				 TEEC_NONE, TEEC_NONE);
	// op.params[0].value.a = 2000;
	// res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_PAY, &op,
	// 			 &err_origin);
	// if (res != TEEC_SUCCESS)
	// 	errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
	// 		res, err_origin);
	
	// Check balance to make sure no money was taken:
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	res = TEEC_InvokeCommand(&sess, TA_DIGITAL_WALLET_VIEW_BALANCE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	printf("Current balance is: %d\n", op.params[0].value.a);


	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */
	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}