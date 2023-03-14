#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,  //0
  HEX_NUM, NUM, REG,//1
  RPAREN, LPAREN,   //2
  ADD, SUB,         //3
  MUL, DIV,         //4
  OR,               //5
  AND,              //6
  TK_EQ, NEQ,       //7
  DEREF, NEG, NOT   //8

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", ADD},         // plus
  {"==", TK_EQ},        // equal
  {"0[xX][0-9A-Fa-f]+", HEX_NUM},
  {"[0-9]+", NUM},
  {"\\-", SUB}, 
  {"\\*", MUL},
  {"\\/", DIV},
  {"\\(", LPAREN},
  {"\\)", RPAREN},
  {"\\$(e)?[abcd]x", REG},
  {"\\$(e)?[bs]p", REG},
  {"\\$(e)?[sd]i", REG},
  {"\\$[abcd]l", REG},
  {"\\$[abcd]h", REG},
  {"\\$eip", REG},
  {"&&", AND},
  {"\\|\\|", OR},
  {"!=", NEQ},
  {"!", NOT}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        tokens[nr_token].type =  rules[i].token_type;
        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case NUM:
          case HEX_NUM:
          case REG:
            assert(substr_len < 32);
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token++].str[substr_len] = '\0';
            break;
          default: TODO();
        }
        
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q){
  // if not surrounded by PARENs
  if(tokens[p].type != LPAREN || tokens[q].type != RPAREN)return 0;
  // PARENs valid
  int cnt = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == LPAREN) {
      cnt++;
    }
    if (tokens[i].type == RPAREN) {
      if(cnt > 0)cnt--;
      else return 0;
    }
  }
  if(cnt != 0)return 0;
  // ok
  return 1;
}

int priority_hash(int e){
  if(e <= TK_NOTYPE)return 0;
  if(e <= REG)return 1;
  if(e <= LPAREN)return 2;
  if(e <= SUB)return 3;
  if(e <= DIV)return 4;
  if(e <= OR)return 5;
  if(e <= AND)return 6;
  if(e <= NEQ)return 7;
  return 8;
}

int dominant_operator_position(int p, int q){
  int pos = -1;
  for(int i=p; i<=q; i++){
    if(tokens[i].type <= REG)continue;
    else if(tokens[i].type == LPAREN){
      i=q;
      while(tokens[i].type != RPAREN && i>=-1)i--;
      assert(i >= p);
    }
    else{
      if(pos==-1 || priority_hash(tokens[i].type) <= priority_hash(tokens[pos].type)){
        pos = i;
      }
    }
  }
  assert(pos != -1);
  return pos;
}

uint32_t eval(int p, int q){
  if(p > q)return -1;
  else if(p == q){
    //Log("hello?0x%x\n", cpu.eax);
    switch (tokens[p].type){
    case NUM:
// bad language
      ;
      int num;
      sscanf(tokens[p].str, "%d", &num);
      return num;
    case HEX_NUM:
// bad language
      ;
      int hex;
      sscanf(tokens[p].str, "%x", &hex);
      return hex;
    case REG:
      if (strcmp(tokens[p].str, "$eax") == 0)       return cpu.eax;
      else if (strcmp(tokens[p].str, "$ebx") == 0)  return cpu.ebx;
      else if (strcmp(tokens[p].str, "$ecx") == 0)  return cpu.ecx;
      else if (strcmp(tokens[p].str, "$edx") == 0)  return cpu.edx;
      else if (strcmp(tokens[p].str, "$ax") == 0)   return cpu.gpr[0]._16;
      else if (strcmp(tokens[p].str, "$bx") == 0)   return cpu.gpr[3]._16;
      else if (strcmp(tokens[p].str, "$cx") == 0)   return cpu.gpr[1]._16;
      else if (strcmp(tokens[p].str, "$dx") == 0)   return cpu.gpr[2]._16;
      else if (strcmp(tokens[p].str, "$al") == 0)   return cpu.gpr[0]._8[0];
      else if (strcmp(tokens[p].str, "$bl") == 0)   return cpu.gpr[3]._8[0];
      else if (strcmp(tokens[p].str, "$cl") == 0)   return cpu.gpr[1]._8[0];
      else if (strcmp(tokens[p].str, "$dl") == 0)   return cpu.gpr[2]._8[0];
      else if (strcmp(tokens[p].str, "$ah") == 0)   return cpu.gpr[0]._8[1];
      else if (strcmp(tokens[p].str, "$bh") == 0)   return cpu.gpr[3]._8[1];
      else if (strcmp(tokens[p].str, "$ch") == 0)   return cpu.gpr[1]._8[1];
      else if (strcmp(tokens[p].str, "$dh") == 0)   return cpu.gpr[2]._8[1];
      else if (strcmp(tokens[p].str, "$ebp") == 0)  return cpu.ebp;
      else if (strcmp(tokens[p].str, "$esp") == 0)  return cpu.esp;
      else if (strcmp(tokens[p].str, "$bp") == 0)   return cpu.gpr[5]._16;
      else if (strcmp(tokens[p].str, "$sp") == 0)   return cpu.gpr[4]._16;
      else if (strcmp(tokens[p].str, "$esi") == 0)  return cpu.esi;
      else if (strcmp(tokens[p].str, "$edi") == 0)  return cpu.edi;
      else if (strcmp(tokens[p].str, "$si") == 0)   return cpu.gpr[6]._16;
      else if (strcmp(tokens[p].str, "$di") == 0)   return cpu.gpr[7]._16;
      else if (strcmp(tokens[p].str, "$eip") == 0)  return cpu.eip;

    default:
      assert(0);
      break;
    }
  }
  else if(check_parentheses(p, q))return eval(p+1, q-1);
  else{
    int op = dominant_operator_position(p, q);
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case ADD:
        return val1 + val2;
      case SUB:
        return val1 - val2;
      case MUL:
        return val1 * val2;
      case DIV:
        return val1 / val2;
      case NOT:
        return !val2;
      case AND:
        return val1 && val2;
      case OR:
        return val1 || val2;
      case TK_EQ:
        return val1 == val2;
      case NEQ:
        return val1 != val2;
      case DEREF:
        return vaddr_read(val2, 4);
      case NEG:
        return -val2;
      default:
        assert(0);
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for(int i=0;i<nr_token;i++){
    if(tokens[i].type == MUL && (i==0 || tokens[i-1].type >= RPAREN)){
      tokens[i].type = DEREF;
    }
    if(tokens[i].type == SUB && (i==0 || tokens[i-1].type >= RPAREN)){
      tokens[i].type = NEG;
    }
  }

  return eval(0, nr_token-1);
  //TODO();
}
