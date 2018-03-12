/* Read an XML document from standard input and print an element
   outline on standard output.
   Must be used with Expat compiled for UTF-8 output.
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <expat.h>

#ifdef XML_LARGE_SIZE
# if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#  define XML_FMT_INT_MOD "I64"
# else
#  defineXML_FMT_INT_MOD "ll"
# endif
#else
# define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
# define XML_FMT_STR "ls"
#else
# define XML_FMT_STR "s"
#endif

#define BUFFSIZE        8192

char Buff[BUFFSIZE];
typedef struct {
    unsigned char key[20];
    unsigned char value[100];
}xml_key_value_t;


xml_key_value_t obj_key_value[10];
xml_key_value_t res_key_value[1000];
unsigned char obj_value_count=0;
unsigned char res_value_count=0;
unsigned char res_count=0;


int Depth;
char objprocess = 0;
char resprocess = 0;
static void XMLCALL
start(void *data, const XML_Char *el, const XML_Char **attr)
{
  int i;
  (void)data;

//  for (i = 0; i < Depth; i++)
//    printf("  ");

//  printf("el:% " XML_FMT_STR, el);
  
  if (strcmp("Object",el) == 0){
      objprocess = 1;
  } else if (strcmp("Resources",el) == 0) {
      objprocess = 0;
      resprocess = 1;
  } else if (objprocess && strcmp("Name",el)==0) {
    strcpy(obj_key_value[obj_value_count].key,el);
  } else if(objprocess && strcmp("ObjectID",el)==0) {
    strcpy(obj_key_value[obj_value_count].key,el);
  }/* else if(objprocess && strcmp("Description1",el)==0) {
    strcpy(obj_key_value[obj_value_count].key,el);
  }*/ else if(objprocess && strcmp("MultipleInstances",el)==0) {
    strcpy(obj_key_value[obj_value_count].key,el);
  } else if (resprocess && strcmp("Name",el)==0) {
    strcpy(res_key_value[res_value_count].key,el);
  } else if(resprocess && strcmp("Operations",el)==0) {
    strcpy(res_key_value[res_value_count].key,el);
  } else if(resprocess && strcmp("Type",el)==0) {
    strcpy(res_key_value[res_value_count].key,el);
  }/* else if(resprocess && strcmp("Description",el)==0) {
    strcpy(res_key_value[res_value_count].key,el);
  }*/

  for (i = 0; attr[i]; i += 2) {
    //printf("att: %" XML_FMT_STR "='%" XML_FMT_STR "'", attr[i], attr[i + 1]);
    if (resprocess && strcmp("ID",attr[i])==0) {
        strcpy(res_key_value[res_value_count].key,attr[i]);
        strcpy(res_key_value[res_value_count].value,attr[i + 1]);
        //printf("%s:%s\n",res_key_value[res_value_count].key,res_key_value[res_value_count].value);
        res_value_count++;
        res_count++;
    }
  }

  //printf("\n");
  Depth++;
}

static void XMLCALL
end(void *data, const XML_Char *el)
{
  (void)data;
  (void)el;

  Depth--;
}

static void XMLCALL
data(void *userData,const XML_Char *s,int len)
{
    char str[1024];
    snprintf(str,len+1,"%s",s);
    //printf("data Depth=%d,str=%s\n",Depth,str);
    if (objprocess && strlen(obj_key_value[obj_value_count].key) != 0 && strlen(obj_key_value[obj_value_count].value) == 0)
    {
        strcpy(obj_key_value[obj_value_count].value,str);
        //printf("%s:%s\n",obj_key_value[obj_value_count].key,obj_key_value[obj_value_count].value);
        obj_value_count++;
    }
    else if (resprocess && strlen(res_key_value[res_value_count].key) != 0 && strlen(res_key_value[res_value_count].value) == 0)
    {
        strcpy(res_key_value[res_value_count].value,str);
        //printf("%s:%s\n",res_key_value[res_value_count].key,res_key_value[res_value_count].value);
        res_value_count++;
    }
        
}

typedef enum
{
    LWM2M_TYPE_UNDEFINED = 0,
    LWM2M_TYPE_OBJECT,
    LWM2M_TYPE_OBJECT_INSTANCE,
    LWM2M_TYPE_MULTIPLE_RESOURCE,

    LWM2M_TYPE_STRING,
    LWM2M_TYPE_OPAQUE,
    LWM2M_TYPE_INTEGER,
    LWM2M_TYPE_FLOAT,
    LWM2M_TYPE_BOOLEAN,

    LWM2M_TYPE_OBJECT_LINK
} lwm2m_data_type_t;

int
main(int argc, char *argv[])
{
  XML_Parser p = XML_ParserCreate(NULL);
  (void)argc;
  (void)argv;

  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    exit(-1);
  }

  XML_SetElementHandler(p, start, end);
  XML_SetCharacterDataHandler(p,data);
  FILE *fp = fopen(argv[1],"r");

  for (;;) {
    int done;
    int len;

    len = (int)fread(Buff, 1, BUFFSIZE, fp);
    if (ferror(fp)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }
    done = feof(fp);

    if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr,
              "Parse error at line %" XML_FMT_INT_MOD "u:\n%" XML_FMT_STR "\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }    
    if (done)
      break;
  }
  XML_ParserFree(p);
  #if 0
  printf("Object:\n");
  for (int i=0;i<obj_value_count;i++) {
      printf("  %s:%s\n",obj_key_value[i].key,obj_key_value[i].value);
  }
  printf("Resource:\n");
  for (int i=0;i<res_value_count;i++) {
      printf("  %s:%s\n",res_key_value[i].key,res_key_value[i].value);
  }
  #endif
  unsigned short objId = 0;
  unsigned char isMulti = 0;
  for (int i=0;i<obj_value_count;i++) {
      if (strcmp(obj_key_value[i].key,"ObjectID")==0)
        objId = atoi(obj_key_value[i].value);
      if (strcmp(obj_key_value[i].key,"MultipleInstances")==0) {
        if (strcmp(obj_key_value[i].value,"Multiple")==0)
            isMulti = 1;
      }
  }
  printf("{ %d,%d,%d,%d, {",objId,isMulti,res_count,1);
  for (int i=0;i<res_value_count;) {
    unsigned short resId = 0;
    unsigned char operate_type = 0;
    unsigned char value_type=0;
    if (strcmp(res_key_value[i].key,"ID")==0)
        resId = atoi(res_key_value[i].value);
    if (strcmp(res_key_value[i+2].key,"Operations")==0) {
        if (strcmp(res_key_value[i+2].value,"R")==0)
            operate_type = 1;
        else if (strcmp(res_key_value[i+2].value,"E")==0)
            operate_type = 3;
    }
    if (strcmp(res_key_value[i+3].key,"Type")==0) {
        if (strcmp(res_key_value[i+3].value,"Integer")==0)
            value_type = LWM2M_TYPE_INTEGER;
        else if (strcmp(res_key_value[i+3].value,"Boolean")==0)
            value_type = LWM2M_TYPE_BOOLEAN;
        else if (strcmp(res_key_value[i+3].value,"String")==0)
            value_type = LWM2M_TYPE_STRING;
        else if (strcmp(res_key_value[i+3].value,"Opaque")==0)
            value_type = LWM2M_TYPE_OPAQUE;
        else if (strcmp(res_key_value[i+3].value,"Objlnk")==0)
            value_type = LWM2M_TYPE_OBJECT_LINK;
        else if (strcmp(res_key_value[i+3].value,"Time")==0)
            value_type = LWM2M_TYPE_OBJECT_INSTANCE;
        else if (strcmp(res_key_value[i+3].value,"Float")==0)
            value_type = LWM2M_TYPE_FLOAT;
    }
    if (value_type != 0)
	value_type -= 3;
    i+=4;
    if (i<res_value_count)
        printf("{%d,%d,%d},",resId,operate_type,value_type);
    else
        printf("{%d,%d,%d}",resId,operate_type,value_type);
  }
  printf("}},\n");
  fclose(fp);
  return 0;
}
