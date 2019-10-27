#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <streambuf>
#include <unordered_map>

#include <cstdio>
#include <cstring>

#include <libxml/tree.h>
#include <libxml/HTMLtree.h>

using AllowedElements = std::unordered_map<std::string, std::vector<std::string>>;

const AllowedElements allowed_elements
{
    {"a", {"href", "title"}}, 
    {"abbr", {"title"}},
    {"acronym", {"title"}},
    {"b", {}},
    {"blockquote", {}},
    {"code", {}},
    {"em", {}},
    {"i", {}},
    {"li", {}},
    {"ol", {}},
    {"strong", {}},
    {"ul", {}},
    {"p", {}},
    {"div", {"class", "id"}},
    {"span", {"class", "id"}},
    {"br", {}},
    {"ruby", {}},
    {"rt", {}}
};

std::vector<xmlElementType> allowed_element_types {
    XML_ELEMENT_NODE, 
    XML_ATTRIBUTE_NODE, 
    XML_TEXT_NODE, 
    XML_CDATA_SECTION_NODE, 
    XML_ENTITY_REF_NODE,
    XML_ENTITY_NODE, 
    // XML_PI_NODE, 
    // XML_COMMENT_NODE, 
    // XML_DOCUMENT_NODE, 
    // XML_DOCUMENT_TYPE_NODE, 
    // XML_DOCUMENT_FRAG_NODE, 
    // XML_NOTATION_NODE, 
    // XML_HTML_DOCUMENT_NODE, 
    // XML_DTD_NODE, 
    // XML_ELEMENT_DECL, 
    // XML_ATTRIBUTE_DECL, 
    // XML_ENTITY_DECL, 
    // XML_NAMESPACE_DECL, 
    // XML_XINCLUDE_START, 
    // XML_XINCLUDE_END, 
    // XML_DOCB_DOCUMENT_NODE
};

// Assume valid parent for all of these
typedef enum NODE_STATUS {
    HAS_PREV_NEXT_SIBLING = 0b1011,
    HAS_PREV_NEXT_SIBLING_AND_CHILDREN = 0b1111,
    HAS_NEXT_SIBLING = 0b1001,
    HAS_NEXT_SIBLING_AND_CHILDREN = 0b1101,
    HAS_PREV_SIBLING = 0b1010,
    HAS_PREV_SIBLING_AND_CHILDREN = 0b1110,
    HAS_CHILDREN = 0b1100
} NODE_STATUS;

NODE_STATUS reckon_status(xmlNode* current)
{
    return static_cast<NODE_STATUS>(
        current->next != nullptr  
        | ( current->prev != nullptr ) << 1 
        | ( current->children != nullptr ) << 2
        | ( current->parent != nullptr ) << 3
        );
}

constexpr bool escape_tags = true;
void add_escaped_tags_as_siblings(xmlNode* current)
{
    char buffer[100];
    snprintf(buffer, 100, "<%s>", current->name);
    xmlNode* prev_tag = xmlNewText((const xmlChar*) (&buffer[0]));

    snprintf(buffer, 100, "</%s>", current->name);
    xmlNode* next_tag = xmlNewText((const xmlChar*) (&buffer[0]));
    xmlAddPrevSibling(current, prev_tag);
    xmlAddNextSibling(current, next_tag);
}

void filter(xmlNodePtr current, int& indent)
{
    ++indent;
    std::cerr << indent << ". Type: " << current->type << "\n";
    std::cerr << indent << ". Name: " << ( current->name == nullptr ? "No name" : (const char*) current->name ) << "\n";
    std::cerr << indent << ". Children: " << (current->children == nullptr ? "Nope" : "Yep") << "\n";
    std::cerr << indent << ". Last: " << (current->last == nullptr ? "Nope" : "Yep") << "\n";
    std::cerr << indent << ". Parent: " << (current->parent == nullptr ? "Nope" : "Yep") << "\n";
    std::cerr << indent << ". Next: " << (current->next == nullptr ? "Nope" : "Yep") << "\n";
    std::cerr << indent << ". Previous: " << (current->prev == nullptr ? "Nope" : "Yep") << "\n";
    std::cerr << indent << ". Attributes: " << (current->properties == nullptr ? "Nope" : "Yep") << "\n";

    if ( current->properties != nullptr )
    {
        xmlAttr* current_attribute = current->properties;
        while ( current_attribute != nullptr )
        {
            std::cerr << indent << ". Attribute " << current_attribute->name << " is " << current_attribute->children->content << "\n";
            current_attribute = current_attribute->next;
        }
    }

    NODE_STATUS status = reckon_status(current);

    // Completely omit unallowed elements
    if ( std::find(allowed_element_types.begin(),
                   allowed_element_types.end(),
                   current->type)
         == allowed_element_types.end() )
    {
        switch (status)
        {
            case HAS_PREV_NEXT_SIBLING:
            case HAS_PREV_NEXT_SIBLING_AND_CHILDREN:
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                filter(current->next, indent);
                break;
            }
            case HAS_NEXT_SIBLING:
            case HAS_NEXT_SIBLING_AND_CHILDREN:
            {
                current->parent->children = current->next;
                current->next->prev = nullptr;
                filter(current->next, indent);
                break;
            }
            case HAS_PREV_SIBLING:
            case HAS_PREV_SIBLING_AND_CHILDREN:
            {
                current->prev->next = nullptr;
                break;
            }
            case HAS_CHILDREN:
            {
                current->parent->children = nullptr;
                break;
            }
        }
        xmlFreeNode(current);
        return;
    }

    // Continue walking, and attach the children correctly to their new parent
    if ( current->type == XML_ELEMENT_NODE )
    {
        std::cerr << "Got an element node\n";

        AllowedElements::const_iterator result = allowed_elements.find(std::string {(const char*) current->name});

        if ( result == allowed_elements.cend() )
        {
            std::cerr << "Unallowed tag!\n";

            if ( escape_tags )
            {
                add_escaped_tags_as_siblings(current);
                status = reckon_status(current);
            }

            switch (status)
            {
                case HAS_PREV_NEXT_SIBLING:
                case HAS_PREV_NEXT_SIBLING_AND_CHILDREN:
                case HAS_PREV_SIBLING:
                case HAS_PREV_SIBLING_AND_CHILDREN:
                {
                    xmlNode* previous = current->prev;
                    xmlNode* next = current->next;
                    xmlNode* parent = current->parent;
                    xmlNode* children = current->children;

                    {
                        xmlUnlinkNode(current);
                        current->children = nullptr;
                        current->last = nullptr;
                        xmlFreeNode(current);
                    }

                    parent->last = previous;
                    previous->next = nullptr;

                    // http://www.xmlsoft.org/html/libxml-tree.html#xmlAddNextSibling
                    // As a result of text merging @elem may be freed.
                    xmlAddChildList(parent, children); // TODO: Test this with null children
                    xmlAddChildList(parent, next);

                    current = previous->next;
                    break;
                }
                case HAS_NEXT_SIBLING:
                case HAS_NEXT_SIBLING_AND_CHILDREN:
                {
                    xmlNode* next = current->next;
                    xmlNode* parent = current->parent;
                    xmlNode* children = current->children;

                    {
                        xmlUnlinkNode(current);
                        current->children = nullptr;
                        current->last = nullptr;
                        xmlFreeNode(current);
                    }

                    parent->children = nullptr;
                    parent->last = nullptr;
                    xmlAddChildList(parent, children);
                    xmlAddChildList(parent, next);

                    current = parent->children;
                    break;
                }
                case HAS_CHILDREN:
                {
                    xmlNode* parent = current->parent;
                    xmlUnlinkNode(current);
                    xmlAddChildList(parent, current->children);

                    {
                        xmlUnlinkNode(current);
                        current->children = nullptr;
                        xmlFreeNode(current);
                    }

                    current = parent->children;
                    break;
                }
                default:
                {
                    throw std::logic_error("You should never have come here\n");
                }
            }
        }
        else if ( current->properties != nullptr )
        {
            xmlAttr* current_attribute = current->properties;

            const std::vector<std::string>& allowed_attributes = result->second;

            while ( current_attribute != nullptr )
            {
                std::vector<std::string>::const_iterator result
                    = std::find(allowed_attributes.cbegin(),
                                allowed_attributes.cend(),
                                (const char*) current_attribute->name);

                if ( result == allowed_attributes.cend() )
                {
                    if ( current_attribute->next != nullptr ) 
                    {
                        xmlAttr* old_attribute = current_attribute;
                        current_attribute = current_attribute->next;

                        // http://www.xmlsoft.org/html/libxml-tree.html#xmlRemoveProp
                        // Unlink and free one attribute
                        xmlRemoveProp(old_attribute);
                    }
                    else
                    {
                        xmlRemoveProp(current_attribute);
                        break;
                    }
                }
                else
                {
                    current_attribute = current_attribute->next;
                }
            }
        }
    }

    if ( current != nullptr and
         current->children != nullptr ) 
    {
        filter(current->children, indent);
    }

    if ( current != nullptr and
         current->next != nullptr )
    {
        filter(current->next, indent);
    }
                                  
    --indent;
}

void test2()
{
    std::string str;
    {
        std::ifstream t("bad.html");

        t.seekg(0, std::ios::end);   
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
    }

    htmlDocPtr document = htmlReadDoc((const xmlChar*) str.c_str(),
                                      nullptr, nullptr,
                                      HTML_PARSE_NODEFDTD | HTML_PARSE_NONET | HTML_PARSE_NOIMPLIED);

    xmlNodePtr current = xmlDocGetRootElement(document);

    int indent = 0;
    filter(current, indent);

    htmlDocDump(stdout, document);
    xmlFreeDoc(document);
}

void test1()
{
    htmlDocPtr document = htmlNewDocNoDtD(nullptr, nullptr);
    xmlNodePtr root = xmlNewNode(nullptr, (const xmlChar*) "div");
    xmlDocSetRootElement(document, root);

    xmlNodePtr text_alpha = xmlNewText((const xmlChar*) "Here is some shit, cabron");
    xmlNodePtr text_beta = xmlNewText((const xmlChar*) "And some more for ya");
    xmlNodePtr gotcha = xmlNewNode(nullptr, (const xmlChar*) "p");

    xmlAddChild(root, text_alpha);
    xmlAddChild(root, gotcha);
    xmlAddChild(root, text_beta);

    htmlDocDump(stdout, document);

    xmlFreeDoc(document);
}

// int main()
// {
//     test2();
//     return 0;
// }
