#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <iterator>

namespace json {
    const double TOL = 0.000001;

    class Node;
// Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Number = std::variant<int, double>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

//-----------------------------
//------------ Node------------
//-----------------------------
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    class Node final : private Value {
    public:

        using variant::variant;

        const Value& GetValue() const { return *this; }

        bool IsInt() const;
        bool IsDouble() const; // Возвращает true, если в Node хранится int либо double.
        bool IsPureDouble() const; // Возвращает true, если в Node хранится double.
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const; // Возвращает значение типа double, если внутри хранится double либо int. В последнем случае возвращается приведённое в double значение.
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        bool operator==(const Node& other) const {
            if (this == &other) {
                return true;
            }
            if ( (IsInt() && other.IsInt()) && (AsInt() == other.AsInt()) ){
                return true;
            }
            if( (IsPureDouble() && other.IsPureDouble()) && (AsDouble() - other.AsDouble() <= TOL) ){
                return true;
            }
            if( (IsBool() && other.IsBool()) && (AsBool() == other.AsBool()) ){
                return true;
            }
            if( (IsString() && other.IsString()) && (AsString() == other.AsString()) ){
                return true;
            }
            if( (IsNull() && other.IsNull()) ){
                return true;
            }
            if(  (IsArray() && other.IsArray()) && (AsArray() == other.AsArray()) ){
                return true;
            }
            if(  (IsMap() && other.IsMap()) && (AsMap() == other.AsMap()) ){
                return true;
            }
            return false;
        }

        bool operator!=(const Node& other) const {
            return !(*this == other);
        }
    };

    //-----------------------------------
    // Вывод данных из JSON
    //-----------------------------------

    // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

//        PrintContext(std::ostream& o, int is, int i): out(o), indent_step(is), indent(i){};

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return {out, indent_step, indent_step + indent};
        }
    };

//    template <typename Value>
//    void PrintValue(const Value& value, const PrintContext& ctx) {
//    ...
//    }

    //----------------------
    //------ Document ------
    //----------------------
    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& other){
            if (this == &other){
                return true;
            }
            return this->root_ == other.GetRoot();
        }

        bool operator!=(const Document& other){
            return !(this->root_ == other.GetRoot());
        }

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json
