#ifndef __HANDLE_REQUEST_H__
#include "handle-request.h"
#endif

#define kBeginScenario  "[\"begin_scenario\"]"
#define kEndScenario    "[\"end_scenario\"]"
#define kStepMatch      "[\"step_matches\""
#define kSnippet        "[\"snippet_text\""
#define kInvoke         "[\"invoke\""

static ProtocolPacket protocolPackets[] = {
    kBeginScenario, 
    kEndScenario,
    kStepMatch,
    kSnippet,
    kInvoke
};

int handleRequest(char *buffer, wire_context *context)
{
    int found;
    int arrayLen = sizeof(protocolPackets)/sizeof(ProtocolPacket);

    if(!context || !buffer || !*buffer)
    {
        return(1);
    }

    for(found = 0; found < arrayLen; found++)
    {
        char *packet = protocolPackets[found].packet;
        if(strstr(buffer, packet))
        {
            break;
        }
    }
    if(found < arrayLen)
    {
        switch(found)
        {
            case 0:                 // begin_scenario           ["begin_scenario"]
                strcpy(buffer, handle_callback(context->begin_callback, context));
                break;

            case 1:                 // end_scenario             ["end_scenario"]
                strcpy(buffer, handle_callback(context->end_callback, context));
                break;

            case 2:                 // step_match               ["step_matches",{"name_to_match":"we're all wired"}]
                                    // "[\"success\",[{\"id\":\"1\", \"args\":[]}]]\n"
                if(context->step_match_callback)
                {
                    strcpy(context->incoming, buffer);
                    strcpy(context->request_block.step_match.name_to_match, getNameToMatch(buffer));
                    (*context->step_match_callback) (context);
                    strcpy(buffer, context->outgoing);
                }
                else
                {
                    strcpy(buffer, "[\"fail\",{\"message\":\"Wire does not implement step_match\"}]\n");
                }
                break;

            case 3:                 // snippet
                                    // ["snippet_text",{"step_keyword":"Given","step_name":"wire server is running","multiline_arg_class":""}]
                if(context->step_snippet_callback)
                {
                    strcpy(context->incoming, buffer);
                    strcpy(context->request_block.step_match.name_to_match, getSnippetToMatch(buffer));
                    (*context->step_snippet_callback) (context);
                    strcpy(buffer, context->outgoing);
                }
                else
                {
                    strcpy(buffer, "[\"fail\",{\"message\":\"Wire does not implement step_snippet\"}]\n");
                }
                break;

            case 4:                 // invoke
                if(context->invoke_callback)
                {
                    strcpy(context->incoming, buffer);
                    context->request_block.step_invoke.id = getIDToInvoke(buffer);
                    int retVal = (*context->invoke_callback) (context);
                    if(retVal == 0)
                    {
                        strcpy(buffer, "[\"success\"]\n");
                    }
                    else
                    {
                        sprintf(buffer, "[\"fail\",{\"message\":\"Failed with %d\"}]\n", retVal);
                    }
                }
                else
                {
                    strcpy(buffer, "[\"fail\",{\"message\":\"Wire does not implement invoke\"}]\n");
                }
                break;

            default:                // Unknown
                strcpy(buffer, "[\"fail\",{\"message\":\"Cucumber sent us an unimplemented command\"}]\n");
                break;                
        }
    }
    else
    {
        strcpy(buffer, "[\"fail\",{\"message\":\"Cucumber sent us an unknown command\"}]\n");
    }
    LOG(buffer)
    return(0);
}
