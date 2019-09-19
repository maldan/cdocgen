#include "osoyanlib/osoyan.h"

struct StringArray *get_function_param_types(char *line) {
    struct StringArray *tuple = chars_split(line, ", ", 0);
    return tuple;
}

struct String *get_function_return(char *line) {
    struct StringArray *tuple = chars_split(line, " ", 0);
    size_t oldLength = 0;
    for (size_t i = 0; i < tuple->length; ++i) {
        if (chars_index_of(tuple->list[i]->list, "(") > 0) {
            oldLength = tuple->length;
            tuple->length = i + 1;

            for (size_t j = 0; j < tuple->list[i]->length; ++j) {
                if (tuple->list[i]->list[j] != '*') {
                    tuple->list[i]->list[j] = 0;
                    break;
                }
            }

            struct String *gaso = string_array_join(tuple, " ");
            tuple->length = oldLength;
            DESTROY_STRING_ARRAY(tuple);
            return gaso;
        }
    }
    DESTROY_STRING_ARRAY(tuple);
    return 0;
}

char *get_function_name(char *line) {
    struct StringArray *tuple = chars_split(line, " ", 0);
    for (size_t i = 0; i < tuple->length; ++i) {
        if (chars_index_of(tuple->list[i]->list, "(") > 0) {
            char *gaso = chars_substr(tuple->list[i]->list, 0, chars_index_of(tuple->list[i]->list, "("));
            DESTROY_STRING_ARRAY(tuple);
            // if (gaso[0] == '*') gaso += 1;
            return gaso;
        }
    }
    DESTROY_STRING_ARRAY(tuple);
    return "sas";
}

void generate_doc(char *fileName, struct String *documentData, char *document) {
    bool isEntityForComment = false;
    bool isStartOfComment = false;
    bool isFirstParameter = false;
    bool isCommentMode = false;
    bool isFileDescription = false;
    bool isFunctionListInit = false;

    string_add(documentData, "{ \"documentName\": \"%s\",\n", fileName);

    NEW_STRING_ARRAY(paramNames);
    NEW_STRING_ARRAY(paramDescription);

    struct StringArray *lines = chars_split(document, "\r\n", 0);
    for (size_t i = 0; i < lines->length; ++i) {
        char *lineData = lines->list[i]->list;

        if (isEntityForComment) {
            // Get function name
            char *functionName = get_function_name(lineData);
            if (functionName[0] == '*') string_add(documentData, "\"function\": \"%s\",\n", functionName + 1);
            else string_add(documentData, "\"function\": \"%s\",\n", functionName);
            MEMORY_FREE(functionName);

            // Get params
            char *params = chars_substr(lineData, chars_index_of(lineData, "(") + 1, strlen(lineData) - 2);
            struct StringArray *s = get_function_param_types(params);
            string_put(documentData, "\"params\": [\n");
            for (size_t j = 0; j < s->length; ++j) {
                char *removeParamName = chars_substr(s->list[j]->list, 0, chars_index_of(s->list[j]->list, paramNames->list[j]->list));
                string_add(documentData, "[\"%s\",", removeParamName);
                string_add(documentData, "\"%s\",", paramNames->list[j]->list);
                string_add(documentData, "\"%s\"],\n", paramDescription->list[j]->list);
                MEMORY_FREE(removeParamName);
            }
            MEMORY_FREE(params);
            DESTROY_STRING_ARRAY(s);

            string_array_clear(paramNames);
            string_array_clear(paramDescription);
            string_put(documentData, "\n], \n");

            // Return type
            struct String *returnType = get_function_return(lineData);
            string_add(documentData, "\"returnType\": \"%s\",\n", returnType->list);
            DESTROY_STRING(returnType);

            string_put(documentData, "},\n");
            isEntityForComment = false;
            isStartOfComment = false;
            isFirstParameter = false;
            isCommentMode = false;
            continue;
        }

        // Obtain comments
        if (isStartOfComment) {
            // If parameter
            if (strncasecmp(lineData, " * @param ", 10) == 0) {
                if (isCommentMode) string_put(documentData, "`, \n");
                isCommentMode = false;

                /*if (!isFirstParameter) {
                    isFirstParameter = true;
                    continue;
                }*/

                // Parse description of params
                struct StringArray *tuple = chars_split(lineData + 10, " - ", 0);
                string_array_push(paramNames, tuple->list[0]->list);
                if (tuple->length > 1) string_array_push(paramDescription, tuple->list[1]->list);
                else string_array_push(paramDescription, "-");
                DESTROY_STRING_ARRAY(tuple);
            } else if (strncasecmp(lineData, " * @return", 10) == 0) {
                if (isCommentMode) string_put(documentData, "`, \n");
                isCommentMode = false;
                string_add(documentData, "\"returnDescription\": `%s`, \n", lineData + 10);
            } else if (strncasecmp(lineData, " * ", 3) == 0) {
                string_put(documentData, lineData + 3);
                string_put(documentData, "\\n");
            }
        }

        // If first line then it description of file
        if (strncasecmp(lineData, "/**", 3) == 0 && i == 0) {
            string_put(documentData, "\"fileDescription\": `");
            isFileDescription = true;
            isStartOfComment = true;
            isCommentMode = true;
            continue;
        }

        // Start of comment
        if (strncasecmp(lineData, "/**", 3) == 0) {
            if (!isFunctionListInit) {
                isFunctionListInit = true;
                string_put(documentData, "\"functionList\": [\n");
            }
            string_put(documentData, "{\n");
            string_put(documentData, "\"description\": `");

            isFirstParameter = false;
            isStartOfComment = true;
            isCommentMode = true;
            continue;
        }

        if (strncasecmp(lineData, " */", 3) == 0) {
            if (isCommentMode) string_put(documentData, "`, \n");
            isCommentMode = false;
            isStartOfComment = false;
            if (!isFileDescription) isEntityForComment = true;
            isFileDescription = false;
            continue;
        }
    }
    if (isFunctionListInit) string_put(documentData, "\n],\n");
    string_add(documentData, "}, \n", fileName);

    DESTROY_STRING_ARRAY(paramNames);
    DESTROY_STRING_ARRAY(paramDescription);
    DESTROY_STRING_ARRAY(lines);
}

int main(int argc, char **argv) {
    MEMORY_INIT;

    // Parse args
    EQU_ARGS(argList) = args_init(argc, argv);

    // Include folder
    if (!args_has_key(argList, "include")) {
        printf("Key --include not found");
        exit(1);
    }

    /*if (argc < 2) {
        printf("Program require args");
        return 1;
    }*/
    // printf("Hello, World!\n");

    // struct Vector *sas = file_search("../osoyan/", "^[a-zA-Z0-9]+\\.h$", FILE_INFO_INCLUDE_DATA);
    struct Vector *sas = file_search(args_get_key_value(argList, "include"), "^[a-zA-Z0-9]+\\.h$", FILE_INFO_INCLUDE_DATA);
    printf("Scanning folder %s ...\n", args_get_key_value(argList, "include"));
    NEW_STRING(documentData);
    NEW_STRING(finalData);

    struct FileInfo *styleCss;
    struct FileInfo *mainJs;
    struct FileInfo *vueJs;
    struct FileInfo *mainHtml;

    if (access( "/var/lib/cdocgen/resource/main.html", F_OK) != -1) {
        styleCss = file_get_contents("/var/lib/cdocgen/resource/style.css");
        mainJs = file_get_contents("/var/lib/cdocgen/resource/main.js");
        vueJs = file_get_contents("/var/lib/cdocgen/resource/vue.js");
        mainHtml = file_get_contents("/var/lib/cdocgen/resource/main.html");
    } else {
        styleCss = file_get_contents("../resource/style.css");
        mainJs = file_get_contents("../resource/main.js");
        vueJs = file_get_contents("../resource/vue.js");
        mainHtml = file_get_contents("../resource/main.html");
    }
    /*NEW_FILE_INFO(styleCss, "../resource/style.css");
    NEW_FILE_INFO(mainJs, "../resource/main.js");
    NEW_FILE_INFO(vueJs, "../resource/vue.js");
    NEW_FILE_INFO(mainHtml, "../resource/main.html");*/

    // Parse document
    if (args_has_key(argList, "verbose")) {
        for (size_t i = 0; i < sas->length; ++i) {
            struct FileInfo *fileInfo = sas->list[i];
            PRINT(fileInfo);
            generate_doc(fileInfo->fileName, documentData, fileInfo->data);
        }
    }

    string_add(finalData, mainHtml->data, styleCss->data, vueJs->data, documentData->list, mainJs->data);

    /*string_put(documentData, "<html>\n");
    string_add(documentData, "<style>\n%s\n</style>\n", styleCss->data);
    string_add(documentData, "<script>%s</script>\n", vueJs->data);
    string_put(documentData, "<body style=\"font-size: 14px;font-family: -apple-system, BlinkMacSystemFont, 'San Francisco', 'Segoe UI', Roboto, Ubuntu, 'Helvetica Neue', Arial, sans-serif;color: #333;\">\n");
    string_put(documentData, "<div id=\"app\"><div v-for=\"item in headerList\"><header-file v-bind:item=\"item\"></header-file></div></div>\n");
    string_put(documentData, "<script>let info = [\n");

    for (size_t i = 0; i < sas->length; ++i) {
        struct FileInfo *fileInfo = sas->list[i];
        PRINT(fileInfo);
        generate_doc(fileInfo->fileName, documentData, fileInfo->data);
    }

    string_put(documentData, "]</script>\n");

    string_add(documentData, "<script>\n%s\n</script>\n", mainJs->data);
    string_put(documentData, "\n</body></html>");*/

    // Include folder
    char *tempOut = "doc.html";

    if (args_has_key(argList, "out")) {
        tempOut = args_get_key_value(argList, "out");
    }
    file_put_contents(tempOut, finalData->list, finalData->length);
    printf("Docs generated to %s\n", tempOut);

    DESTROY_STRING(documentData);
    DESTROY_STRING(finalData);
    DESTROY_FILE_SEARCH_RESULT(sas);
    DESTROY_FILE_INFO(styleCss);
    DESTROY_FILE_INFO(mainJs);
    DESTROY_FILE_INFO(vueJs);
    DESTROY_FILE_INFO(mainHtml);
    args_free(argList);

    MEMORY_PRINT_STATE;

    return 0;
}
