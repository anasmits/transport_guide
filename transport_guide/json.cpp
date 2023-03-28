#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            bool closing_simbol = false;
            for (char c; input >> c || c != ']';) {
                if ( c == ']'){
                    closing_simbol = true;
                    break;
                }
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if(!closing_simbol){
                throw ParsingError("Array: no closing ']'"s);
            }
            return Node(std::move(result));
        }

        using Number = std::variant<int, double>;

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            } else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return Node(std::stoi(parsed_num)); //std::stoi(parsed_num);
                    } catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::stod(parsed_num));// std::stod(parsed_num);
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        /*std::string*/ Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                } else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                        case 'n':
                            s.push_back('\n');
                            break;
                        case 't':
                            s.push_back('\t');
                            break;
                        case 'r':
                            s.push_back('\r');
                            break;
                        case '"':
                            s.push_back('"');
                            break;
                        case '\\':
                            s.push_back('\\');
                            break;
                        default:
                            // Встретили неизвестную escape-последовательность
                            throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                } else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                } else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }
            return Node(std::move(s));
        }

        Node LoadDict(istream& input) {
            Dict result;

            bool closing_simbol = false;
            for (char c; input >> c || c == '}';) {
                if(c == '}'){
                    closing_simbol = true;
                    break;
                }
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({std::move(key), LoadNode(input)});
            }
            if(!closing_simbol){
                throw ParsingError("Dictionary: no closing '}'"s);
            }
            return Node(std::move(result));
        }

        Node LoadNull(istream& input){
            char s[5];
            try {
                input.get(s, 5);
                if(s == "null"s) {
                    return Node(nullptr);// std::stod(parsed_num);
                } else{
                    throw  ParsingError("Null value is wrong"s);
                }
            } catch (...) {
                throw ParsingError("Null value is wrong"s);
            }
        }

        Node LoadBool(istream& input){
            char c = input.peek();
            if (c == 't'){
                char s[5];
                try {
                    input.get(s, 5);
                    if(s == "true"s) {
                        return Node(true);// std::stod(parsed_num);
                    } else{
                        throw  ParsingError("Bool value is wrong"s);
                    }
                } catch (...) {
                    throw ParsingError("Bool value is wrong"s);
                }
            }
            if (c == 'f'){
                char s[6];
                try {
                    input.get(s, 6);
                    if(s == "false"s) {
                        return Node(false);// std::stod(parsed_num);
                    } else{
                        throw  ParsingError("Bool value is wrong"s);
                    }
                } catch (...) {
                    throw ParsingError("Bool value is wrong"s);
                }
            }
            throw ParsingError("Bool value is wrong"s);
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            } else if (c == '{') {
                return LoadDict(input);
            } else if (c == '"') {
                return LoadString(input);
            } else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            } else if (c == 't' || c == 'f'){
                input.putback(c);
                return LoadBool(input);
            } else {
                input.putback(c);
                return LoadNumber(input);  //return LoadInt(input);
            }
        }

    }  // namespace

    //-------------------------------------------------
    //              Node functions
    //-------------------------------------------------

    //----------------- bool Is...()-------------------

    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    //----------------- value As...()-------------------

    int Node::AsInt() const {
        if (holds_alternative<int>(*this)) {
            return get<int>(*this);
        }
        throw std::logic_error("Wrong value type: Int"s);
    }

    bool Node::AsBool() const {
        if (holds_alternative<bool>(*this)) {
            return get<bool>(*this);
        }
        throw std::logic_error("Wrong value type: Bool"s);
    }

    double Node::AsDouble() const {
        if (holds_alternative<double >(*this)) {
            return get<double>(*this);
        }
        if (holds_alternative<int>(*this)){
            return static_cast<double>(get<int>(*this));
        }
        throw std::logic_error("Wrong value type: Double"s);
    }

    const string& Node::AsString() const {
        if (holds_alternative<string>(*this)) {
            return get<string>(*this);
        }
        throw std::logic_error("Wrong value type: String"s);
    }

    const Array& Node::AsArray() const {
        if (holds_alternative<Array>(*this)) {
            return get<Array>(*this);
        }
        throw std::logic_error("Wrong value type: Array"s);
    }

    const Dict& Node::AsMap() const {
        if (holds_alternative<Dict>(*this)) {
            return get<Dict>(*this);
        }
        throw std::logic_error("Wrong value type: Dict"s);
    }

    //-------------------------------------------------
    // Перегрузки функции PrintValue
    //-------------------------------------------------

    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, std::ostream& out) {
        using namespace std::literals;
        out << "null"sv;
    }

    void PrintValue(std::nullptr_t, PrintContext& context) {
        using namespace std::literals;
        context.out << "null"sv;
    }

    // Перегрузка функции PrintValue для вывода значений int
    void PrintValue(const int& value, std::ostream& out) {
        out << value;
    }

    // Перегрузка функции PrintValue для вывода значений double
    void PrintValue(const double& value, std::ostream& out) {
        out << value;
    }

    // Перегрузка функции PrintValue для вывода значений string
    std::string filter(char c) {
        using namespace std;
        switch (c) {
            case '\n':
                return "\\n"s;
            case '\r':
                return "\\r"s;
            case '\t':
                return "\t"s;
            case '\"':
                return "\\\""s;
            case '\\':
                return "\\\\"s;
            default:
                return {c};
        }
    }

    void PrintValue(const std::string& s, std::ostream& out) {
        out << "\"";
        std::transform(
                std::begin(s),
                std::end(s),
                std::ostream_iterator<std::string>(out),
                filter
        );
        out << "\"";
    }

    // Перегрузка функции PrintValue для вывода значений bool
    void PrintValue(const bool& value, std::ostream& out) {
        using namespace std::literals;
        if(value){
            out << "true"sv;
        } else {
            out << "false"sv;
        }
    }

    void PrintValue(const Array& ar, std::ostream& out);
    void PrintValue(const Dict& dict, std::ostream& out);

    // Перегрузка функции PrintValue для вывода значений Node
    void PrintNode(const Node& node, std::ostream& out) {
        std::visit(
                [&out](const auto& value){ PrintValue(value, out); }, // - original
                node.GetValue());
    }

    // Перегрузка функции PrintValue для вывода значений Array
    void PrintValue(const Array& ar, std::ostream& out) {
        out << "["sv;
        for(size_t i = 0; i < ar.size(); ++i){
            PrintNode(ar[i], out);
            if (i < ar.size()-1) {
                out << ","sv;
            }
        }
        out << "]"sv;
    }

    // Перегрузка функции PrintValue для вывода значений Dict
    void PrintValue(const Dict& dict, std::ostream& out) {
        bool first = true;
        out << "{ "sv;
        for(const auto& [key, val] : dict){
            if(first){
                out << "\"" << key << "\"" <<  ": "sv ;
                PrintNode(val, out);
                first = false;
            } else{
                out << ", "sv;
                out << "\"" << key << "\"" << ": "sv ;
                PrintNode(val, out);
            }
        }
        out << " } "sv;
    }

    //-------------------------------------------------
    //              Document
    //-------------------------------------------------

    Document::Document(Node root)
            : root_(std::move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{LoadNode(input)};
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), output);
    }


}  // namespace json
