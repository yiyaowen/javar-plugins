#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "com_yiyaowen_javar_c_SyntaxParser.h"

typedef struct {
    int start[1000];
    int end[1000];
    int count;
} KeywordInfo, QuoteInfo, CommentInfo;

typedef struct {
    KeywordInfo * aKeywordInfo;
    QuoteInfo quoteInfo;
    CommentInfo commentInfo;
} SyntaxParseInfo;

typedef struct {
    const char * text;
    int length;
} Keyword;

SyntaxParseInfo parseFile(const char * file, int size);

// Config
int kwTotalCount;
Keyword * keywords;
int ssTotalCount;
char * splitSymbols;

/*
 * WARNING : Conflict - C_char and Java_char
 *
 * TO BE COMPLETED
 */

JNIEXPORT void JNICALL Java_com_yiyaowen_javar_c_1SyntaxParser_fillSyntaxParseInfo (JNIEnv * env, jclass cls, jobject j_info, jstring j_file, jobjectArray j_keywords, jint j_kwTotalCount, jcharArray j_splitSymbols, jint j_ssTotalCount)
{
    ////////////////////
    // Initialization //
    ////////////////////
    
    kwTotalCount = j_kwTotalCount;
    keywords = (Keyword *) malloc(sizeof(Keyword)*kwTotalCount);
    for (int i = 0; i < kwTotalCount; ++i) {
        keywords[i].text = (*env)->GetStringUTFChars(env, (jstring) (*env)->GetObjectArrayElement(env, j_keywords, i), 0);
        keywords[i].length = strlen(keywords[i].text);
    }
    const char * file = (*env)->GetStringUTFChars(env, j_file, 0);
    ssTotalCount = j_ssTotalCount;
    splitSymbols = (char *) (*env)->GetCharArrayElements(env, j_splitSymbols, NULL);

    /* DEBUG */
    for (int i = 0; i < ssTotalCount; ++i) {
        printf("[%d]", i);
        if (splitSymbols[i] == '\n') { printf("\\n"); }
        else if (splitSymbols[i] == '\t') { printf("\\t"); }
        else if (splitSymbols[i] == ' ') { printf("_"); }
        else if (splitSymbols[i] == EOF) { printf("\n"); }
        else { printf("%c", splitSymbols[i]); }
    }
    /* DEBUG */
    
    /////////////////
    // Timer start //
    /////////////////

    time_t timeStart = time(NULL);
    
    /////////////////
    // Start parse //
    /////////////////

    SyntaxParseInfo info = parseFile(file, strlen(file));
    
    //////////////////////
    // Fill information //
    //////////////////////
    
    // Get SyntaxParseInfo
    jclass j_SyntaxParseInfoClass = (*env)->FindClass(env, "com/yiyaowen/javar/c_SyntaxParseInfo");
    jmethodID setAKeywordInfoAtID = (*env)->GetMethodID(env, j_SyntaxParseInfoClass, "setAKeywordInfoAt", "(Lcom/yiyaowen/javar/c_SingleInfo;I)V");
    jmethodID setQuoteInfoID = (*env)->GetMethodID(env, j_SyntaxParseInfoClass, "setQuoteInfo", "(Lcom/yiyaowen/javar/c_SingleInfo;)V");
    jmethodID setCommentInfoID = (*env)->GetMethodID(env, j_SyntaxParseInfoClass, "setCommentInfo", "(Lcom/yiyaowen/javar/c_SingleInfo;)V");
    // Get SingleInfo
    jclass j_SingleInfoClass = (*env)->FindClass(env, "com/yiyaowen/javar/c_SingleInfo");
    jmethodID ctorID = (*env)->GetMethodID(env, j_SingleInfoClass, "<init>", "()V");
    jmethodID setStartAtID = (*env)->GetMethodID(env, j_SingleInfoClass, "setStartAt", "(II)V");
    jmethodID setEndAtID = (*env)->GetMethodID(env, j_SingleInfoClass, "setEndAt", "(II)V");
    jmethodID setCountID = (*env)->GetMethodID(env, j_SingleInfoClass, "setCount", "(I)V");
        // Set aKeywordInfo
    for (int i = 0; i < kwTotalCount; ++i) {
        jobject j_keywordInfo = (*env)->NewObject(env, j_SingleInfoClass, ctorID);
        (*env)->CallVoidMethod(env, j_keywordInfo, setCountID, info.aKeywordInfo[i].count);
        for (int j = 0; j < info.aKeywordInfo[i].count; ++j) {
            (*env)->CallVoidMethod(env, j_keywordInfo, setStartAtID, info.aKeywordInfo[i].start[j], j);
            (*env)->CallVoidMethod(env, j_keywordInfo, setEndAtID, info.aKeywordInfo[i].end[j], j);
        }
        (*env)->CallVoidMethod(env, j_info, setAKeywordInfoAtID, j_keywordInfo, i);
    }
        // Set quoteInfo
    jobject j_quoteInfo = (*env)->NewObject(env, j_SingleInfoClass, ctorID);
    (*env)->CallVoidMethod(env, j_quoteInfo, setCountID, info.quoteInfo.count);
    for (int i = 0; i < info.quoteInfo.count; ++i) {
        (*env)->CallVoidMethod(env, j_quoteInfo, setStartAtID, info.quoteInfo.start[i], i);
        (*env)->CallVoidMethod(env, j_quoteInfo, setEndAtID, info.quoteInfo.end[i], i);
    }
    (*env)->CallVoidMethod(env, j_info, setQuoteInfoID, j_quoteInfo);
        // Set commentInfo
    jobject j_commentInfo = (*env)->NewObject(env, j_SingleInfoClass, ctorID);
    (*env)->CallVoidMethod(env, j_commentInfo, setCountID, info.commentInfo.count);
    for (int i = 0; i < info.commentInfo.count; ++i) {
        (*env)->CallVoidMethod(env, j_commentInfo, setStartAtID, info.commentInfo.start[i], i);
        (*env)->CallVoidMethod(env, j_commentInfo, setEndAtID, info.commentInfo.end[i], i);
    }
    (*env)->CallVoidMethod(env, j_info, setCommentInfoID, j_commentInfo);
    
    //////////////////////
    // Release resource //
    //////////////////////
    
    (*env)->ReleaseStringUTFChars(env, j_file, file);
    for (int i = 0; i < kwTotalCount; ++i) {
        (*env)->ReleaseStringUTFChars(env, (jstring) (*env)->GetObjectArrayElement(env, j_keywords, i), keywords[i].text);
    }

    ///////////////
    // Timer end //
    ///////////////

    time_t timeEnd = time(NULL);

    printf("Elapsed time: %ld s\n", timeEnd-timeStart);
}

/**
 * Parse file syntax and return parse result
 *
 * @param file (File to be parsed)
 * @param size (File length)
 * @return info (File parse result)
 */
SyntaxParseInfo parseFile(const char * file, int size)
{
#define NORMAL      0
#define IN_QUOTE            1
#define IN_LINE_COMMENT     2
#define IN_MUL_COMMENT      3

    int state = NORMAL;
    int i, quote_s, quote_e, comment_s, comment_e;

    // Initialize info
    SyntaxParseInfo info;
    info.aKeywordInfo = (KeywordInfo *) malloc(sizeof(KeywordInfo)*kwTotalCount);
    info.quoteInfo.count = 0;
    info.commentInfo.count = 0;
    for (int j = 0; j < kwTotalCount; ++j) {
        info.aKeywordInfo[j].count = 0;
    }

    for (i = 0; i < size; ++i) {
        /* DEBUG */
        /*if (file[i] == ' ') { printf("_"); }
        else if (file[i] == '\t') { printf("\\t"); }
        else if (file[i] == '\n') { printf("\\n"); }
        else if (file[i] == EOF) { printf("\n"); }
        else { printf("%c", file[i]); }*/
        /* DEBUG */
        if (state != IN_QUOTE) {
            /* NOT IN_QUOTE */
            if (state == IN_MUL_COMMENT) {
                /* IN_MUL_COMMENT */
                if (file[i] == '/' && file[i-1] == '*') {
                    comment_e = i;
                    info.commentInfo.start[info.commentInfo.count] = comment_s;
                    info.commentInfo.end[info.commentInfo.count] = comment_e;
                    ++info.commentInfo.count;
                    state = NORMAL;
                }
                /* IN_MUL_COMMENT */
            }
            else {
                /* NOT IN_MUL_COMMENT */
                if (state == IN_LINE_COMMENT) {
                    /* IN_LINE_COMMENT */
                    if (file[i] == '\n' || file[i] == EOF) {
                        comment_e = i;
                        info.commentInfo.start[info.commentInfo.count] = comment_s;
                        info.commentInfo.end[info.commentInfo.count] = comment_e;
                        ++info.commentInfo.count;
                        state = NORMAL;
                    }
                    /* IN_LINE_COMMENT */
                }
                else {
                    /* NOT IN_LINE_COMMENT */
                    if (file[i] == '"') {
                        if (file[i-1] != '\'' && file[i+1] != '\'') {
                            quote_s = i; 
                            state = IN_QUOTE;
                        }
                    }
                    else if (file[i] == '\'') {
                        if (file[i+1] == '\'') {
                            quote_s = i;
                            state = IN_QUOTE;
                        }
                    }
                    else if (file[i] == '/' && file[i-1] == '/') {
                        comment_s = i-1;
                        state = IN_LINE_COMMENT;
                    }
                    else if (file[i] == '*' && file[i-1] == '/') {
                        comment_s = i-1;
                        state = IN_MUL_COMMENT;
                    }
                    else {
                        /* NORMAL */
                        for (int j = 0; j < kwTotalCount; ++j) {
                            if (strncmp(&file[i], keywords[j].text, keywords[j].length) == 0) {
                                bool leftIsolated = false;
                                bool rightIsolated = false;
                                for (int k = 0; k < ssTotalCount; ++k) {
                                    if (file[i-1] == splitSymbols[k]) {
                                        leftIsolated = true;
                                    }
                                    if (file[i+keywords[j].length] == splitSymbols[k]) {
                                        rightIsolated = true;
                                    }
                                }
                                if (leftIsolated && rightIsolated) {
                                    info.aKeywordInfo[j].start[info.aKeywordInfo[j].count] = i;
                                    info.aKeywordInfo[j].end[info.aKeywordInfo[j].count] = i+keywords[j].length-1;
                                    ++info.aKeywordInfo[j].count;
                                    break;
                                }
                            }
                        }
                        /* NORMAL */
                    }
                    /* NOT IN_LINE_COMMENT */
                }
                /* NOT IN_MUL_COMMENT */
            }
            /* NOT IN_QUOTE */
        }
        else {
            /* IN_QUOTE */
            if (file[i] == '"') {
                if (file[i-1] != '\'' && file[i+1] != '\'') {
                    quote_e = i;
                    info.quoteInfo.start[info.quoteInfo.count] = quote_s;
                    info.quoteInfo.end[info.quoteInfo.count] = quote_e;
                    ++info.quoteInfo.count;
                    state = NORMAL;
                }
            }
            else if (file[i] == '\'') {
                if (file[i-2] == '\'') {
                    quote_e = i;
                    info.quoteInfo.start[info.quoteInfo.count] = quote_s;
                    info.quoteInfo.end[info.quoteInfo.count] = quote_e;
                    ++info.quoteInfo.count;
                    state = NORMAL;
                }
            }
            /* IN_QUOTE */
        }
    }

    return info;
}
