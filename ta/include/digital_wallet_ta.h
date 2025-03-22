#ifndef TA_DIGITAL_WALLET_H
#define TA_DIGITAL_WALLET_H


/*
 * This UUID is generated with uuidgen
 * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
 */
#define TA_DIGITAL_WALLET_UUID \
	{ 0x20a7c644, 0x583b, 0x4914, \
        { 0xab, 0x08, 0x07, 0x47, 0xc1, 0x1a, 0x60, 0x39 } }

/* The function IDs implemented in this TA */
#define TA_DIGITAL_WALLET_DEPOSIT	            0
#define TA_DIGITAL_WALLET_PAY		            1
#define TA_DIGITAL_WALLET_VIEW_BALANCE		    2
#define TA_DIGITAL_WALLET_CHECK_TRANSACTION		3
#define MAX_STRING_SIZE                         1024 

typedef enum
{
	PAYMENT,
	DEPOSIT

}TRANSACTION_TYPE;

// the next node will technically be the previous transaction so
// we are calling it previous_transaction even though it is the next node
typedef struct Transaction {
    int amount;
    int remaining_amount;
    int transaction_number;
    TRANSACTION_TYPE transaction_type;
    struct Transaction* previous_transaction; // store a pointer to the next node (address of next Transaction struct)
} Transaction;

typedef struct {
    Transaction* head; // Pointer to the first node in the list
} TransactionList;


void init_balance(int* balance);
void init_num_transactions(int* num_transactions);

void init_list(TransactionList* transactions);
void add_transaction(TransactionList* list, int amount, TRANSACTION_TYPE transaction, int remaining_amount, int transaction_number);
void free_list(TransactionList* transactions);
const char* get_transactions(TransactionList* transactions);

#endif /*TA_DIGITAL_WALLET_H*/