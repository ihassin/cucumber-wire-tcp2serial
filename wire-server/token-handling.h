#ifndef __TOKEN_HANDLING_H__
#define __TOKEN_HANDLING_H__

int getIDToInvoke(char *buffer);
char *getNameToMatch(char *buffer);
char *getSnippetToMatch(char *buffer);
char *getGherkinVar(char *buffer);
int getGherkinVarPosition(char *buffer, char *var);
char *getJSONArg(char *buffer);

#endif
