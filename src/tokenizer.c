#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "tokenizer.h"
#include "tokens.h"

// Below are reserved keywords.
// TODO: Move this to tokens.h ? or better manage this.
static const char *R_Keywords[KWORDS_SIZE] = {
    "print", "if", "else"
};

int build_tokens(char *buff, TokenMgr *tokmgr) {
	if (buff == NULL) {
		printf("** Error Buffer invalid state cannot build tokens\n");
		return 1;
	}

	if (tokmgr == NULL) {
		printf("** Error Invalid token manager passed to build token");
		return 1;
	}

	// Each character in buffer.
	char c;
	// Reusable storage to hold strings. 
	char store[100];
	// Buff iterator.
	int bidx = 0;
	// Track storage size.
	int stctr = 0;
	// Error code.
	int error = 0;
	// Track line no.
	int lineno = 1;
	
	// Iterate through all chars until terminator or error.
	while (buff[bidx] != '\0' && !error) {
		c = buff[bidx];

		if (c == COMMENT) {
			while (c != NEWLINE && c != '\0') {
				c = buff[++bidx];
			}
			continue;

		} 
		else if (c == NEWLINE) {
			lineno++;
			bidx++;
			continue;

		}
		else if (c == BANG) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, "OPERATOR", "NOTEQUALTO");
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, "OPERATOR", "NOT");
			}
			bidx++;
		}
		else if (isspace(c)) {
			bidx++;
			continue;

		}
		else if (c == LPAREN) {
			TokenMgr_add_token(tokmgr, "LPAREN", "(");
			bidx++;

		}
		else if (c == RPAREN) {
			TokenMgr_add_token(tokmgr, "RPAREN", ")");
			bidx++;
		}
		else if (c == LESSTHAN) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, "OPERATOR", "LTEQTO");
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, "OPERATOR", "LESSTHAN");
			}
			bidx++;
		}
		else if (c == GREATERTHAN) {
			switch(buff[++bidx]) {
				case EQUAL:
					TokenMgr_add_token(tokmgr, "OPERATOR", "GTEQTO");
					break;
				case LESSTHAN:
					TokenMgr_add_token(tokmgr, "OPERATOR", "BETWEEN");
					break;
				default:
					bidx--;
					TokenMgr_add_token(tokmgr, "OPERATOR", "GREATERTHAN");
					break;
			}
			bidx++;
		}
		else if (c ==  EQUAL) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, "OPERATOR", "EQUALTO");
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, "OPERATOR", "EQUAL");
			}	
			bidx++;
		}
		else if (c == PLUS) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "PLUS");
			bidx++;
		} 
		else if (c ==  MINUS) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "MINUS");
			bidx++;
		} 
		else if (c ==  ASTERISK) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "ASTERISK");
			bidx++;
		} 
		else if (c == FSLASH) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "FSLASH");
			bidx++;
		}
		else if (c == DQUOTE) {
			c = buff[++bidx];
			while (c != DQUOTE && c != '\0' && c != NEWLINE) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			
			// Ensure last read char is closing quote.
			if (c != DQUOTE) {
				error = 1;
				continue;
			}
			TokenMgr_add_token(tokmgr, "STRING", store);
			stctr = 0;
			bidx++;
		}
		else if (c == VAR) {
			c = buff[++bidx];
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "IDENTIFIER", store);
			stctr = 0;
		}
		else if (isdigit((int) c)) {
			while (isdigit((int) c)) {
				store[stctr++] = c;
				c = (int) buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "INTEGER", store);
			stctr = 0;
		}
		else if (c == LBRACE) {
			c = buff[++bidx];
			
			// Catch use cases where { NON-ALPHA }  
			if (!is_valid_identifier(c)) {
				store[stctr++] = c;
				store[stctr] = '\0';
				error = 1;
				continue;
			}

			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}

			store[stctr] = '\0';
			// Ensure last read char is right brace.
			if (c != RBRACE) {
				error = 1;
				continue;
			}

			TokenMgr_add_token(tokmgr, "GROUP", store);
			stctr = 0;
			bidx++;
		}
		else if (isalpha(c)) {
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = (int) buff[++bidx];
			}
			store[stctr] = '\0';
			if (is_valid_keyword(store)) {
				TokenMgr_add_token(tokmgr, "KEYWORD", store);
			}
			else {
				error = 1;
			}
			stctr = 0;
		}
		else {
			store[stctr++] = c;
			c = buff[++bidx];
			while (c != COMMENT && c != LBRACE && c != RBRACE \
				&& c != DQUOTE && c != VAR & c != NEWLINE && c!= EQUAL) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			error = 1;
		}
	}

	if (error)
		printf("** Invalid syntax: unknown '%s' found in line %d\n", store, lineno);

	return error;
}

TokenMgr *TokenMgr_new() {
	TokenMgr *tok_mgr = malloc(sizeof(TokenMgr));
	tok_mgr->tok_ctr = 0;
	tok_mgr->curr_tok = NULL;
	return tok_mgr;
}

Token *TokenMgr_current_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return NULL;

	return *tok_mgr->curr_tok;
}

int TokenMgr_add_token(TokenMgr *tok_mgr, char tok_type[50], char tok_val[100]) {
	if (tok_mgr == NULL) {
		printf("**Error** Invalid token manager passed to TokenMgr_add_token");
		return 1;
	}

	Token *tmp = malloc(sizeof(Token));
	strcpy(tmp->type, tok_type);
	strcpy(tmp->value, tok_val);
	tmp->val_length = strlen(tmp->value);
	tok_mgr->toks[tok_mgr->tok_ctr] = tmp;
	tok_mgr->tok_ctr += 1;
	return 0;
}

void TokenMgr_print_tokens(TokenMgr *tok_mgr) {
	for (size_t i =0; i < tok_mgr->tok_ctr; i++) {
		printf("%s %s \n", tok_mgr->toks[i]->type, tok_mgr->toks[i]->value);
	}
}

int TokenMgr_free(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		printf("**Error** Invalid token manager passed to TokenMgr_free");
		return 1;
	}

	for (size_t i = 0; i < tok_mgr->tok_ctr; i++) {
		free(tok_mgr->toks[i]);
	}
	tok_mgr->curr_tok = NULL;
	free(tok_mgr);
	return 0;
}

Token *TokenMgr_next_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return NULL;

	if (tok_mgr->curr_tok == NULL) {
		tok_mgr->curr_tok = tok_mgr->toks;
	} 
	else {

		// Don't surpass final token.
		if (*tok_mgr->curr_tok == tok_mgr->toks[tok_mgr->tok_ctr-1]) {
			return NULL;
		}
		
		tok_mgr->curr_tok++;	
	}
	return *tok_mgr->curr_tok;
}

Token *TokenMgr_prev_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return NULL;

	if (tok_mgr->curr_tok == NULL) {
		tok_mgr->curr_tok = &tok_mgr->toks[tok_mgr->tok_ctr-1];
	} 
	else {

		// Don't surpass first token.
		if (*tok_mgr->curr_tok == tok_mgr->toks[0]) {
			return NULL;
		}
			
		tok_mgr->curr_tok--;	
	}
	return *tok_mgr->curr_tok;
}

void TokenMgr_reset_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return;
	tok_mgr->curr_tok = NULL;
}

void TokenMgr_clear_tokens(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		printf("**Error** Invalid token manager passed to TokenMgr_free");
		return;
	}
	tok_mgr->curr_tok = NULL;
	tok_mgr->tok_ctr = 0;
}

Token *get_first_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL || tok_mgr->tok_ctr < 1)
		return NULL;
	
	return tok_mgr->toks[0];
}

Token *get_last_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL || tok_mgr->tok_ctr < 1)
		return NULL;

	return tok_mgr->toks[tok_mgr->tok_ctr-1];
}

int is_valid_keyword(char *str) {
	int ret = 0;
	
	if (str == NULL)
		return ret;
	
	for (int x = 0; x < KWORDS_SIZE; x++) {
		const char *t = R_Keywords[x];
		if (strcmp(t, str) == 0) {
			ret = 1;
			break;
		}
	}
	return ret;
}
