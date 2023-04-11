#include "json_builder.h"

namespace json {

    using namespace std::literals;

    //---------------Builder------------
        Node Builder::Build() {
            return root_node_;
        }

        Builder& Builder::Value(Node::Value value){
            if (nodes_stack_.empty()){
                if (ready){
                    throw std::logic_error("error adding Value() after Value"s);
                }
                root_node_ = std::move(value);
                ready = true;
            } else {
                if (nodes_stack_.back()->IsArray()){
                    auto ar = std::get<json::Array>(nodes_stack_.back()->GetValueNotConst());
                    ar.emplace_back(Node(value));
                    *nodes_stack_.back() = ar;
                } else {
                    if (!key_.has_value()){
                        throw std::logic_error("Adding Value() to Dict without Key()"s);
                    }
                    auto dict = std::get<json::Dict>(nodes_stack_.back()->GetValueNotConst());
                    dict.emplace(std::make_pair(key_.value(), Node(value)));
                    *nodes_stack_.back() = dict;
                    key_.reset();
                }
            }
            return *this;
        }

        KeyItemContext Builder::Key(std::string s){
            if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
                throw std::logic_error("Adding Key() without Dict");
            }
            key_.emplace(s);
            auto dict = std::get<json::Dict>(nodes_stack_.back()->GetValueNotConst());
            dict.insert({s, Node()});
            return KeyItemContext(*this);
        }

        ArrayItemContext Builder::StartArray() {
            if (ready){
                throw std::logic_error("error adding Array()"s);
            }
            if (nodes_stack_.empty()){
                nodes_stack_.emplace_back(new Node(Array()));
            } else {
                if (nodes_stack_.back()->IsArray()){
                    auto ar = std::get<json::Array>(nodes_stack_.back()->GetValueNotConst());
                    ar.emplace_back(Array());
                    *nodes_stack_.back() = std::move(ar);
                    nodes_stack_.emplace_back(&std::get<json::Array>(nodes_stack_.back()->GetValueNotConst()).back());
                } else if ((nodes_stack_.back()->IsDict() && key_.has_value()) ){
                    auto dict = std::get<json::Dict>(nodes_stack_.back()->GetValueNotConst());
                    dict.insert({key_.value(), Array()});
                    *nodes_stack_.back() = std::move(dict);
                    nodes_stack_.emplace_back(&std::get<json::Dict>(nodes_stack_.back()->GetValueNotConst()).at(key_.value()));
                    key_.reset();
                } else {
                    throw std::logic_error("error StartArray()");
                }
            }
            return ArrayItemContext(*this);
        }

        DictItemContext Builder::StartDict() {
            if (ready){
                throw std::logic_error("error adding Dict()"s);
            }
            if (nodes_stack_.empty()){
                nodes_stack_.emplace_back(new Node(Dict()));
            } else {
                if ((nodes_stack_.back()->IsDict() && key_.has_value()) ){
                    auto dict = std::get<json::Dict>(nodes_stack_.back()->GetValueNotConst());
                    dict.insert({key_.value(), Dict()});
                    *nodes_stack_.back() = std::move(dict);
                    nodes_stack_.emplace_back(&std::get<json::Dict>(nodes_stack_.back()->GetValueNotConst()).at(key_.value()));
                    key_.reset();
                } else if (nodes_stack_.back()->IsArray()){
                    auto ar = std::get<json::Array>(nodes_stack_.back()->GetValueNotConst());
                    ar.emplace_back(Dict());
                    *nodes_stack_.back() = std::move(ar);
                    nodes_stack_.emplace_back(&std::get<json::Array>(nodes_stack_.back()->GetValueNotConst()).back());
                } else {
                    throw std::logic_error("error StartDict()");
                }
            }
            return DictItemContext(*this);
        }

        Builder& Builder::EndDict(){
            if ( (!nodes_stack_.empty() && !nodes_stack_.back()->IsDict()) || ready) {
                throw std::logic_error("Error end Dict");
            }
            if (nodes_stack_.size() == 1){
                if(key_.has_value()){
                    throw std::logic_error("Error end Dict: the Key() is not closed");
                }
                root_node_ = *nodes_stack_[0];
                ready = true;
            }
            nodes_stack_.pop_back();

            return *this;

        }

        Builder& Builder::EndArray(){
            if ( (!nodes_stack_.empty() && !nodes_stack_.back()->IsArray()) || ready) {
                throw std::logic_error("Error end Array");
            }
            if (nodes_stack_.size() == 1){
                root_node_ = *nodes_stack_[0];
                ready = true;
            }
            nodes_stack_.pop_back();

            return *this;
        }

        std::vector<Node *> &Builder::GetNodeStack() {
            return nodes_stack_;
        }

        std::optional<std::string> &Builder::GetKey() {
            return key_;
        }

        bool &Builder::GetReady() {
            return ready;
        }

        json::Node &Builder::GetRoot() {
            return root_node_;
        }

        //----------DictValueContext-----------

        KeyItemContext DictItemContext::Key(std::string s){
            return KeyItemContext(builder_ref_.Key(s));
        }
        Builder& DictItemContext::EndDict(){
            return builder_ref_.EndDict();
        }

        //----------KeyItemContext------------
        ValueItemContextAfterKey KeyItemContext::Value(const Node::Value &&value){
            return ValueItemContextAfterKey(builder_ref_.Value(value));
        }
        DictItemContext KeyItemContext::StartDict(){
            return builder_ref_.StartDict();
        }
        ArrayItemContext KeyItemContext::StartArray(){
            return builder_ref_.StartArray();
        }

        //----------ValueItemContextAfterArray------------
        ValueItemContextAfterArray ValueItemContextAfterArray::Value(Node::Value value){
            return ValueItemContextAfterArray(builder_ref_.Value(value));
        }
        DictItemContext ValueItemContextAfterArray::StartDict(){
            return builder_ref_.StartDict();
        }
        ArrayItemContext ValueItemContextAfterArray::StartArray(){
            return builder_ref_.StartArray();
        }
        Builder& ValueItemContextAfterArray::EndArray(){
            return builder_ref_.EndArray();
        }

        //----------ValueItemContextAfterKey-----------
        KeyItemContext ValueItemContextAfterKey::Key(std::string s){
            return KeyItemContext(builder_ref_.Key(s));
        }

        Builder& ValueItemContextAfterKey::EndDict(){
            return builder_ref_.EndDict();
        }

        //----------ArrayItemContext------------
        ValueItemContextAfterArray ArrayItemContext::Value(Node::Value value){
            return ValueItemContextAfterArray(builder_ref_.Value(value));
        }

        DictItemContext ArrayItemContext::StartDict(){
            return builder_ref_.StartDict();
        }
        ArrayItemContext ArrayItemContext::StartArray(){
            return builder_ref_.StartArray();
        }

        Builder& ArrayItemContext::EndArray(){
            return builder_ref_.EndArray();
        }
} // namespace json
