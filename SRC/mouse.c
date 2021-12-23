#include "mouse.h"

const char *tagnames[]={ "MACRO", "PARAM", "LOOP"};

/**********/
void ERROR(char *s)
{
    if(s!=NULL) { printf("%s\n",s); }

    exit(EXIT_FAILURE);
}

/**********/
int NUM(char c)
{
    return c - ASCII_UPPER_OFFSET;
}

/**********/
int VAL(char c)
{
    return c - ASCII_NUMBER_OFFSET;
}

/**********/
void GETCHAR(mouse *m)
{
    if(m->CHPOS>=PROG_SIZE){ ERROR(ERROR_PROG_END); }
    else
    {
        m->CH = m->PROG[m->CHPOS];
        m->CHPOS++; 
    }
}

/**********/
void PUSHCAL(mouse *m, int d)
{
    if(m->CAL>=STACK_SIZE){ printf("PUSCHAL\n"); ERROR(ERROR_STACK_FULL); }
    else
    {
        m->CALSTACK[m->CAL] = d;
        m->CAL++;
    }
}

/**********/
int POPCAL(mouse *m)
{
    if(m->CAL<=0){ ERROR(ERROR_STACK_EMPTY); return 0; }
    else
    {
        m->CAL--;
        return m->CALSTACK[m->CAL];
    }
}

/**********/
void PUSH(mouse *m, tagtype tag)
{
    if(m->LEVEL>=STACK_SIZE){ printf("PUSH\n"); ERROR(ERROR_STACK_FULL); }
    else
    {
        m->STACK[m->LEVEL].tag = tag;
        m->STACK[m->LEVEL].pos = m->CHPOS;
        m->STACK[m->LEVEL].off = m->OFFSET;
        m->LEVEL++;
    }
}

/**********/
void POP(mouse *m)
{
    if(m->LEVEL<=0){ ERROR(ERROR_STACK_EMPTY); }
    else
    {
        m->LEVEL--;
        m->CHPOS = m->STACK[m->LEVEL].pos;
        m->OFFSET = m->STACK[m->LEVEL].off;
    }
}

/*********/
void SKIP(mouse *m, char lch, char rch)
{
    int cnt = 1;
    do
    {
        GETCHAR(m);

        if(m->CH == lch){ cnt++; }
        else
        {
            if(m->CH == rch){ cnt--; }
        }

    } while(cnt!=0);
}

/**********/
void LOAD(mouse *m, char *file)
{
    FILE *fp = fopen(file,"rb");
    
    if(fp!=NULL)
    {
        if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); ERROR(ERROR_FILE_READING); };

        size_t size = (size_t)ftell(fp);

        if(size<=PROG_SIZE) 
        { 
            if (fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); ERROR(ERROR_FILE_READING); };

            size_t check = fread(m->PROG, size, 1, fp);

            if (check != 1) { fclose(fp); ERROR(ERROR_FILE_READING); };

            fclose(fp);
        }
        else
        {
            fclose(fp); ERROR(ERROR_PROG_SIZE); 
        }
    }
    else
    {
        ERROR(ERROR_FILE_NOT_FOUND);
    }

}

/**********/
void INIT(mouse *m)
{
    int pos = -1;
    char last = 0;
    char this = 0;
    
    do
    {
        last = this;

        pos++;

        this = m->PROG[pos];

        if( last=='$' && this>='A' && this<='Z' ) 
        { 
            m->DEFS[NUM(this)] = pos+1;
        } 

    } while ( !((this=='$') && (last=='$')) && pos<=PROG_SIZE);
}

/**********/
void RUN(mouse *m)
{
    do
    {
        GETCHAR(m);
        
        switch(m->CH)
        {
            
            case ']': case '$': case ' ': case '\t': case '\r':
            break;

            case '\n':
            {
                m->LINE++;
            }
            break;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            {   
                int tmp=0;
                while(isdigit(m->CH))
                {
                    tmp= tmp*10+ VAL(m->CH);
                    GETCHAR(m);
                }
                PUSHCAL(m,tmp);
                m->CHPOS--;
            }
            break;

            case 'A': case 'B': case 'C': case 'D': case 'E':
            case 'F': case 'G': case 'H': case 'I': case 'J':
            case 'K': case 'L': case 'M': case 'N': case 'O':
            case 'P': case 'Q': case 'R': case 'S': case 'T':
            case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            {   
                PUSHCAL(m, NUM(m->CH) + m->OFFSET);
            }
            break;

            case '?':
            {
                int n = 0;
                int err = scanf("%d",&n);
                if(err!=1){ ERROR(ERROR_INPUT);}
                PUSHCAL(m,n);
            }
            break;

            case '!':
            {
                int n = POPCAL(m);
                printf("%d",n);
            }
            break;

            case '+':
            {
                PUSHCAL(m, POPCAL(m)+POPCAL(m));
            }
            break;

            case '-':
            {
                int b = POPCAL(m);
                int a = POPCAL(m);

                PUSHCAL(m, a-b);
            }
            break;

            case '*':
            {
                PUSHCAL(m, POPCAL(m)*POPCAL(m));
            }
            break;

            case '/':
            {
                int b = POPCAL(m);
                int a = POPCAL(m);

                PUSHCAL(m, a/b);
            }
            break;

            case '.':
            {
                PUSHCAL(m,m->DATA[POPCAL(m)]); 
            }
            break;

            case '=':
            {
                int tmp = POPCAL(m);
                m->DATA[POPCAL(m)] = tmp;
            }
            break;

            case '"':
            {
               do
               {
                   GETCHAR(m);
                   if(m->CH=='!'){ printf("\n");}
                   else 
                   {
                       if(m->CH!='"'){ printf("%c",m->CH); }
                   }
               } while (m->CH!='"');
            }
            break;

            case '[':
            {
                if(POPCAL(m)<=0){ SKIP(m,'[', ']'); }
            }
            break;

            case '(':
            {
                PUSH(m, LOOP);
            }
            break;

            case '^':
            {
                if(POPCAL(m)<=0)
                { 
                    POP(m);
                    SKIP(m,'(', ')'); 
                }
            }
            break;

            case ')':
            {
                m->CHPOS = m->STACK[m->LEVEL-1].pos;
            }
            break;

            case '#':
            {
                GETCHAR(m);

                if(m->DEFS[NUM(m->CH)] >= 0)
                {
                    PUSH(m,MACRO);
                    m->CHPOS = m->DEFS[NUM(m->CH)];
                    m->OFFSET += ASCII_SIZE; 
                }
                else
                {
                    SKIP(m,'#',';');
                }
            }
            break;

            case '@':
            {
                POP(m);
                SKIP(m,'#',';');
            }
            break;

            case '%'  :
            {
                GETCHAR(m); 
                
                int parnum = NUM(m->CH); 

                PUSH(m,PARAM);
                   
                int parbal = 1; 
                int tmp = m->LEVEL-1;

                do 
                {
                    tmp--;
                    
                    switch (m->STACK[tmp].tag)
                    {
                        case MACRO : parbal--; break;
                        case PARAM : parbal++; break;
                        case LOOP : break;
                        default: ERROR(ERROR_TAG);
                    }
                } while (parbal != 0);
                
                m->CHPOS = m->STACK[tmp].pos;
                m->OFFSET = m->STACK[tmp].off;
                
                do 
                {
                    GETCHAR(m);

                    if(m->CH == '#')
                    {
                        SKIP(m,'#',';'); 
                        GETCHAR(m);
                    }
                    
                    if (m->CH == ',') 
                    {
                        parnum--;
                    }

                } while (parnum >= 0 && m->CH != ';');
                
                if (m->CH == ';') 
                {
                    POP(m);
                }
            }    
            break;

            case ',': case ';':
            {
                POP(m);
            }
            break;

            case '\'':
            {
                do
                {
                    GETCHAR(m);

                } while (m->CH!='\n');
            }
            break;

            default:
                printf("Line : %d, Pos : %d\n", m->LINE, m->CHPOS-1);
                printf("m->CH : %c\n", m->CH);
                ERROR(ERROR_UNKNOW_SYMBOL);
            break;
        }

    } while (m->CH!='$');
    
}
