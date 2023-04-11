#pragma once
#include "json.h"
#include <vector>
#include <optional>
#include <variant>


namespace json {

    class BaseItemContext;
    class DictItemContext;
    class ArrayItemContext;
    class ValueItemContextAfterArray;
    class ValueItemContextAfterKey;
    class KeyItemContext;

    using ItemContextType = std::variant<std::monostate, BaseItemContext, ArrayItemContext, DictItemContext>;


        class Builder {
        public:

            Builder() = default;

            Node Build();
            DictItemContext StartDict();
            ArrayItemContext StartArray();
            KeyItemContext Key(std::string s);
            Builder& EndDict();
            Builder& EndArray();
            Builder& Value(Node::Value value);

            std::vector<Node *> &GetNodeStack();
            std::optional<std::string> &GetKey();
            bool &GetReady();
            json::Node &GetRoot();

        private:
            json::Node root_node_;
            std::vector<Node *> nodes_stack_;
            std::optional<std::string> key_;
            bool ready = false;
        };


    class BaseItemContext {
    public:
        explicit BaseItemContext (Builder& br) : builder_ref_(br) {};
    protected:
        Builder& builder_ref_;
    };

    class DictItemContext : public BaseItemContext{
    public:
        explicit DictItemContext(Builder& builder_ref) : BaseItemContext(builder_ref){};
        KeyItemContext Key(std::string s);
        Builder& EndDict();
    };

    class KeyItemContext : public BaseItemContext {
    public:
        explicit KeyItemContext(Builder& builder_ref) : BaseItemContext(builder_ref){};
        ValueItemContextAfterKey Value(const Node::Value &&value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
    };

    class ArrayItemContext : public BaseItemContext {
    public:
        explicit  ArrayItemContext(Builder& br) : BaseItemContext(br){};
        ValueItemContextAfterArray Value(Node::Value value); //
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndArray();
    };

    class ValueItemContextAfterKey : public BaseItemContext {
    public:
        explicit ValueItemContextAfterKey(Builder& builder_ref) : BaseItemContext(builder_ref){};
        KeyItemContext Key(std::string s);
        Builder& EndDict();
    };

    class ValueItemContextAfterArray : public BaseItemContext {
    public:
        explicit ValueItemContextAfterArray(Builder& builder_ref): BaseItemContext(builder_ref){};

        ValueItemContextAfterArray Value(Node::Value value); //
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndArray();
    };



} // namespace json

