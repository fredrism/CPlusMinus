#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_LITERAL 65536
#define TOKEN_LITERAL_FLOAT 1 + TOKEN_LITERAL
#define TOKEN_LITERAL_INT 2 + TOKEN_LITERAL
#define TOKEN_LITERAL_STRING 3 + TOKEN_LITERAL
#define TOKEN_NAME 1
#define TOKEN_STRUCT 2
#define TOKEN_RETURN 3
#define TOKEN_IF 4
#define TOKEN_ELSE 5
#define TOKEN_NEWLINE 6
#define TOKEN_PREPROCESSOR 7
#define TOKEN_COMMENT 8
#define TOKEN_MULTILINE_COMMENT 9

#define TOKEN_SYMBOL 131072
#define TOKEN_LPAREN 1 + TOKEN_SYMBOL
#define TOKEN_RPAREN 2 + TOKEN_SYMBOL
#define TOKEN_LBRACKET 3 + TOKEN_SYMBOL
#define TOKEN_RBRACKET 4 + TOKEN_SYMBOL
#define TOKEN_LCURLY 5 + TOKEN_SYMBOL
#define TOKEN_RCURLY 6 + TOKEN_SYMBOL
#define TOKEN_SEMICOLON 7 + TOKEN_SYMBOL
#define TOKEN_STAR 8 + TOKEN_SYMBOL
#define TOKEN_LT 9 + TOKEN_SYMBOL
#define TOKEN_GT 10 + TOKEN_SYMBOL
#define TOKEN_EQUAL 11 + TOKEN_SYMBOL
#define TOKEN_EQUALEQUAL 12 + TOKEN_SYMBOL
#define TOKEN_LESSEQUAL 13 + TOKEN_SYMBOL
#define TOKEN_GREATEREQUAL 14 + TOKEN_SYMBOL
#define TOKEN_COMMA 15 + TOKEN_SYMBOL
#define TOKEN_DOT 16 + TOKEN_SYMBOL
#define TOKEN_ARROW 17 + TOKEN_SYMBOL
#define TOKEN_PLUS 18 + TOKEN_SYMBOL
#define TOKEN_MINUS 19 + TOKEN_SYMBOL
#define TOKEN_PLUSPLUS 20 + TOKEN_SYMBOL
#define TOKEN_MINUSMINUS 21 + TOKEN_SYMBOL
#define TOKEN_PLUSMINUS 22 + TOKEN_SYMBOL

#define TOKEN_UNKNOWN 0
#define TOKEN_EOF 69

const char* GetTokenName(int token) {
	switch (token) {
		// Literal tokens
	case 65536: return "TOKEN_LITERAL";
	case 65537: return "TOKEN_LITERAL_FLOAT";
	case 65538: return "TOKEN_LITERAL_INT";
	case 65539: return "TOKEN_LITERAL_STRING";

		// Keyword tokens
	case 1: return "TOKEN_NAME";
	case 2: return "TOKEN_STRUCT";
	case 3: return "TOKEN_RETURN";
	case 4: return "TOKEN_IF";
	case 5: return "TOKEN_ELSE";
	case 6: return "TOKEN_NEWLINE";
	case 7: return "TOKEN_PREPROCESSOR";
	case 8: return "TOKEN_COMMENT";
	case 9: return "TOKEN_MULTILINE_COMMENT";

		// Symbol tokens
	case 131072: return "TOKEN_SYMBOL";
	case 131073: return "TOKEN_LPAREN";
	case 131074: return "TOKEN_RPAREN";
	case 131075: return "TOKEN_LBRACKET";
	case 131076: return "TOKEN_RBRACKET";
	case 131077: return "TOKEN_LCURLY";
	case 131078: return "TOKEN_RCURLY";
	case 131079: return "TOKEN_SEMICOLON";
	case 131080: return "TOKEN_STAR";
	case 131081: return "TOKEN_LT";
	case 131082: return "TOKEN_GT";
	case 131083: return "TOKEN_EQUAL";
	case 131084: return "TOKEN_EQUALEQUAL";
	case 131085: return "TOKEN_LESSEQUAL";
	case 131086: return "TOKEN_GREATEREQUAL";
	case 131087: return "TOKEN_COMMA";
	case 131088: return "TOKEN_DOT";
	case 131089: return "TOKEN_ARROW";
	case 131090: return "TOKEN_PLUS";
	case 131091: return "TOKEN_MINUS";
	case 131092: return "TOKEN_PLUSPLUS";
	case 131093: return "TOKEN_MINUSMINUS";
	case 131094: return "TOKEN_PLUSMINUS";

	case 69: return "TOKEN_EOF";
		// Default case for unknown tokens
	default: return "UNKNOWN_TOKEN";
	}
}

struct Substring {
	char* start;
	unsigned int length;
};

struct Token {
	struct Substring string;
	unsigned int type;
	void* value;
	struct Token* next;
	struct Token* previous;
};

struct LexerContext {
	char* fileBuffer;
	char* tokenBuffer;
	unsigned int fileBufferSize;
	unsigned int tokenBufferSize;
	unsigned int readPos;
	unsigned int tokenLength;
};

char PeekChar(struct LexerContext* ctx)
{
	return ctx->fileBuffer[ctx->readPos];
}

char PeekAhead(struct LexerContext* ctx, int count)
{
	return ctx->fileBuffer[ctx->readPos + count];
}

char GetChar(struct LexerContext* ctx)
{
	return ctx->fileBuffer[ctx->readPos++];
}

int IsLineEnd(char ch)
{
	return ch == '\0' || ch == '\n';
}

void ReadLine(struct LexerContext* ctx, struct Token* token)
{
	token->string.start = ctx->fileBuffer + ctx->readPos;
	while (!IsLineEnd(PeekChar(ctx)))
	{
		ctx->tokenBuffer[ctx->tokenLength] = GetChar(ctx);
		ctx->tokenLength++;
	}

	token->string.length = ctx->tokenLength;
}

int MakeSymbol(struct LexerContext* ctx, unsigned int length, unsigned int type, struct Token* token)
{
	token->string.start = ctx->fileBuffer + ctx->readPos;
	token->string.length = length;
	token->type = type;
	ctx->readPos += length;
	return 1;
}

void ReadName(struct LexerContext* ctx, struct Token* token)
{
	char* start = ctx->fileBuffer + ctx->readPos;

	while (isalnum(PeekChar(ctx)))
	{
		ctx->tokenBuffer[ctx->tokenLength] = GetChar(ctx);
		ctx->tokenLength++;
	}

	ctx->tokenBuffer[ctx->tokenLength] = '\0';

	if (strcmp(ctx->tokenBuffer, "return") == 0)
	{
		token->type = TOKEN_RETURN;
	}
	else if (strcmp(ctx->tokenBuffer, "if") == 0)
	{
		token->type = TOKEN_IF;
	}
	else if (strcmp(ctx->tokenBuffer, "else") == 0)
	{
		token->type = TOKEN_ELSE;
	}
	else if (strcmp(ctx->tokenBuffer, "struct") == 0)
	{
		token->type = TOKEN_STRUCT;
	}
	else
	{
		token->type = TOKEN_NAME;
	}

	token->string.start = start;
	token->string.length = ctx->tokenLength;
}

int ReadString(struct LexerContext* ctx, struct Token* token)
{
	char* start = ctx->fileBuffer + ctx->readPos;
	GetChar(ctx);

	while (PeekChar(ctx) != '\"')
	{
		GetChar(ctx);
		ctx->tokenLength++;
	}

	GetChar(ctx);
	token->string.start = start;
	token->string.length = ctx->tokenLength + 2;
	char* value = malloc(ctx->tokenLength + 1);
	memset(value, 0, ctx->tokenLength + 1);
	memcpy(value, token->string.start + 1, ctx->tokenLength);
	token->type = TOKEN_LITERAL_STRING;
	token->value = value;

	return 1;
}

int ReadNumber(struct LexerContext* ctx, struct Token* token)
{
	char* start = ctx->fileBuffer + ctx->readPos;
	while (isdigit(PeekChar(ctx)))
	{
		ctx->tokenBuffer[ctx->tokenLength] = GetChar(ctx);
		ctx->tokenLength++;
	}

	if (PeekChar(ctx) == '.')
	{
		ctx->tokenBuffer[ctx->tokenLength] = GetChar(ctx);
		ctx->tokenLength++;

		while (isdigit(PeekChar(ctx)))
		{
			ctx->tokenBuffer[ctx->tokenLength] = GetChar(ctx);
			ctx->tokenLength++;
		}

		token->type = TOKEN_LITERAL_FLOAT;
	}
	else
	{
		token->type = TOKEN_LITERAL_INT;
	}

	token->string.start = start;
	token->string.length = ctx->tokenLength;
	return 1;
}

int GetToken(struct LexerContext* ctx, struct Token* token)
{
	char ch = PeekChar(ctx);
	ctx->tokenLength = 0;

	if (ch == '\0')
	{
		MakeSymbol(ctx, 1, TOKEN_EOF, token);
		return 0;
	}

	switch (ch)
	{
	case '#':
		ReadLine(ctx, token);
		token->type = TOKEN_PREPROCESSOR;
		return 1;
	case '/':
		if (PeekAhead(ctx, 1) == '/')
		{
			ReadLine(ctx, token);
			token->type = TOKEN_COMMENT;
			return 1;
		}
		break;
	case '\"':
		return ReadString(ctx, token);
	case '{':
		return MakeSymbol(ctx, 1, TOKEN_LCURLY, token);
	case '}':
		return MakeSymbol(ctx, 1, TOKEN_RCURLY, token);
	case '[':
		return MakeSymbol(ctx, 1, TOKEN_LBRACKET, token);
	case ']':
		return MakeSymbol(ctx, 1, TOKEN_RBRACKET, token);
	case '(':
		return MakeSymbol(ctx, 1, TOKEN_LPAREN, token);
	case ')':
		return MakeSymbol(ctx, 1, TOKEN_RPAREN, token);
	case '*':
		return MakeSymbol(ctx, 1, TOKEN_STAR, token);
	case '=':
		if (PeekAhead(ctx, 1) == '=')
		{
			return MakeSymbol(ctx, 2, TOKEN_EQUALEQUAL, token);
		}
		return MakeSymbol(ctx, 1, TOKEN_EQUAL, token);
	case ';':
		return MakeSymbol(ctx, 1, TOKEN_SEMICOLON, token);
	case ',':
		return MakeSymbol(ctx, 1, TOKEN_COMMA, token);
	case ' ':
		GetChar(ctx);
		return GetToken(ctx, token);
	case '\t':
		GetChar(ctx);
		return GetToken(ctx, token);
	case '\n':
		return MakeSymbol(ctx, 1, TOKEN_NEWLINE, token);
	case '+':
		if (PeekAhead(ctx, 1) == '+')
		{
			return MakeSymbol(ctx, 2, TOKEN_PLUSPLUS, token);
		}
		if (PeekAhead(ctx, 1) == '-')
		{
			return MakeSymbol(ctx, 2, TOKEN_PLUSMINUS, token);
		}
		return MakeSymbol(ctx, 1, TOKEN_PLUS, token);
	case '-':
		if (PeekAhead(ctx, 1) == '>')
		{
			return MakeSymbol(ctx, 2, TOKEN_ARROW, token);
		}
		if (PeekAhead(ctx, 1) == '-')
		{
			return MakeSymbol(ctx, 2, TOKEN_MINUSMINUS, token);
		}
		return MakeSymbol(ctx, 1, TOKEN_MINUS, token);

	}

	if (isalpha(ch))
	{
		ReadName(ctx, token);
		return 1;
	}
	else if (isdigit(ch))
	{
		return ReadNumber(ctx, token);
	}

	MakeSymbol(ctx, 1, TOKEN_UNKNOWN, token);
	return 1;
}

struct ParserContext
{
	struct Token* start;
	int count;
};

struct Expression
{
	struct Token* start;
	struct Token* end;
};

struct ExprNode
{
	struct Expression expr;
	struct ExprNode* next;
};

void AppendToken(struct Expression* expr, struct Token* token)
{
	struct Token* cpy = malloc(sizeof(struct Token));
	memcpy(cpy, token, sizeof(struct Token));
	cpy->next = NULL;
	cpy->previous = NULL;

	if (expr->start == NULL)
	{
		expr->start = cpy;
		expr->end = cpy;
		cpy->next = NULL;
		cpy->previous = NULL;
		return;
	}

	expr->end->next = cpy;
	expr->end = cpy;
}

void PrintToken(struct Token* token)
{
	printf("%d", token->type);
	if (token->type == TOKEN_NEWLINE)
	{
		printf("  %s (\\n)\n", GetTokenName(token->type));
	}
	else
	{
		printf("  %s (%.*s)\n", GetTokenName(token->type), token->string.length, token->string.start);
	}
}

void PrintExpression(struct Expression* expr)
{
	if (expr->start->type == TOKEN_NEWLINE)
	{
		return;
	}

	printf("EXPR : \n");
	struct Token* curr = expr->start;
	while (curr != NULL)
	{
		printf("  ");
		PrintToken(curr);
		curr = curr->next;
	}
}

int GetExpression(struct LexerContext* ctx)
{
	struct ExprNode* file = malloc(sizeof(struct ExprNode));
	file->next = NULL;
	file->expr.start = NULL;
	file->expr.end = NULL;
	struct ExprNode* current = file;
	struct Token token;

	while (GetToken(ctx, &token))
	{
		if (token.type == TOKEN_EOF)
		{
			return 0;
		}

		else if (token.type == TOKEN_NAME)
		{
			AppendToken(&current->expr, &token);
			GetToken(ctx, &token);

			if (token.type == TOKEN_LPAREN)
			{
				AppendToken(&current->expr, &token);
				GetToken(ctx, &token);

				while (token.type != TOKEN_RPAREN)
				{
					AppendToken(&current->expr, &token);
					GetToken(ctx, &token);
				}

				AppendToken(&current->expr, &token);
			}
		}
		else
		{
			AppendToken(&current->expr, &token);
		}

		PrintExpression(&current->expr);
		struct ExprNode* next = malloc(sizeof(struct ExprNode));
		memset(next, 0, sizeof(struct ExprNode));
		current->next = next;
		current = next;
	}

	return 0;
}

int main(int argc, char** argv)
{
	printf(" ---- C+- transpiling transpiler ---- \n");
	if (argc < 2)
	{
		printf("No input files specified, example use \"cpm main.c\"");
		return 0;
	}

	char* filename = argv[1];
	FILE* sourceFile = fopen(filename, "r");

	if (sourceFile == NULL)
	{
		printf("Source file \"%s\" not found.", filename);
		return 0;
	}

	fseek(sourceFile, 0, SEEK_END);
	long fileSize = ftell(sourceFile);
	printf("\"%s\" length: %ld characters.\n", filename, fileSize);

	char* fileBuffer = (char*)malloc(fileSize);
	if (fileBuffer == NULL)
	{
		printf("Failed to allocate file buffer.");
		fclose(sourceFile);
		return 0;
	}

	memset(fileBuffer, 0, fileSize);

	rewind(sourceFile);
	size_t bytesRead = fread(fileBuffer, 1, fileSize, sourceFile);

	char tokenBuffer[512];
	struct LexerContext ctx;
	ctx.tokenBuffer = tokenBuffer;
	ctx.tokenBufferSize = 512;
	ctx.tokenLength = 0;
	ctx.readPos = 0;
	ctx.fileBuffer = fileBuffer;
	ctx.fileBufferSize = bytesRead;

	printf("Starting lexer...\n");
	printf("Tokens: \n");
	struct Token token;

	//FILE* dstFile = fopen("out.c", "w");

	/*
	while(GetToken(&ctx, &token))
	{
		printf("%d", token.type);
		if(token.type == TOKEN_NEWLINE)
		{
			printf("  %s (\\n)\n", GetTokenName(token.type));
		}
		else
		{
			printf("  %s (%.*s)\n", GetTokenName(token.type), token.string.length, token.string.start);
		}

		fprintf(dstFile, "%.*s ", token.string.length, token.string.start);
	}

	if(token.type == TOKEN_EOF)
	{
		printf("EOF\n Finished generating tokens.");
	}
	*/

	GetExpression(&ctx);

	fclose(sourceFile);
	//fclose(dstFile);
	return 0;
}
