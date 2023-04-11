#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

// ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        std::stringstream s;
        if(!points_.empty()){
            s << " "sv;
        }
        s << point.x << ","sv << point.y;
        points_ += s.str();
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        out << points_;
        out << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

// ---------- Text ------------------
    Text& Text::SetPosition(Point point){
        position_ = point;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }
// Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size){
        font_size_ = size;
        return *this;
    }

// Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family){
        font_family_ = font_family;
        return *this;
    }

// Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight){
        font_weight_ = font_weight;
        return *this;
    }

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data){
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(context.out);
        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\""sv;
        out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
        out << " font-size=\""sv << font_size_ << "\""sv;
        if(!font_family_.empty()){
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if(!font_weight_.empty()){
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        out << ">";
        if(!data_.empty()){
            for(const auto s : data_){
                switch (s) {
                    case '"':
                        out << "&quot;"sv;  break;
                    case '<':
                        out << "&lt;"sv;    break;
                    case '>':
                        out << "&gt;"sv;    break;
                    case '\'':
                        out << "&apos"sv;    break;
                    case '&':
                        out << "&amp"sv;    break;
                    default:
                        out << s;   break;
                }
            }
        }
        out << "</text>"sv;

    }

// ---------- Document ------------------

// Добавляет в svg-документ объект-наследник svg::Object
    void Document::AddPtr(std::unique_ptr<Object>&& obj){
        objects_.emplace_back(std::move(obj));
    }

// Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        RenderContext context(out, 2, 2);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objects_){
            obj.get()->Render(context);
        }
        out << "</svg>"sv;
    }
}  // namespace svg
