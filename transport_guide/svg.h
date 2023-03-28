#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <deque>
#include <sstream>
#include <optional>
#include <variant>
#include <vector>

namespace svg {

    struct Point {
        Point() = default;
        Point(double x, double y)
                : x(x)
                , y(y) {
        }
        double x = 0;
        double y = 0;
    };

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
    struct RenderContext {
        RenderContext(std::ostream& out)
                : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
                : out(out)
                , indent_step(indent_step)
                , indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    struct Rgb{
        Rgb() = default;
        Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b){};
        uint8_t red = 0u;
        uint8_t green = 0u;
        uint8_t blue = 0u;
    };

    struct Rgba{
        Rgba() = default;
        Rgba(uint8_t r, uint8_t g, uint8_t b, double o) : red(r), green(g), blue(b), opacity(o){};
        uint8_t red = 0u;
        uint8_t green = 0u;
        uint8_t blue = 0u;
        double opacity = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
    inline const Color NoneColor{};

    struct OstreamColorPrinter {
        std::ostream& out;

        void operator()(const std::monostate) const {
            using namespace std::literals;
            out << "none"sv;
        }
        void operator()(const std::string color) const {
            out << color;
        }
        void operator()(const Rgb rgb) const {
            using namespace std::literals;
            out << "rgb("sv << +rgb.red << ","sv << +rgb.green << ","sv << +rgb.blue << ")"sv;
        }
        void operator()(const Rgba rgba) const {
            using namespace std::literals;
            out << "rgba("sv << +rgba.red << ","sv << +rgba.green << ","sv << +rgba.blue << ","sv << rgba.opacity << ")"sv;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const Color& color){
        visit(OstreamColorPrinter{os}, color);
        return os;
    }
// Объявив в заголовочном файле константу со спецификатором inline,
// мы сделаем так, что она будет одной на все единицы трансляции,
// которые подключают этот заголовок.
// В противном случае каждая единица трансляции будет использовать свою копию этой константы
//    inline const Color NoneColor{"none"};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::ostream& operator<<(std::ostream& out, const svg::StrokeLineCap& line_cap){
        using namespace std::literals;
        switch(line_cap) {
            case StrokeLineCap::BUTT :
                out << "butt"sv;
                break;
            case StrokeLineCap::ROUND :
                out << "round"sv;
                break;
            case StrokeLineCap::SQUARE :
                out << "square"sv;
                break;
            default:
                out << ""sv;
                break;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const svg::StrokeLineJoin& line_join){
        using namespace std::literals;
        switch(line_join) {
            case StrokeLineJoin::ARCS :
                out << "arcs"sv;
                break;
            case StrokeLineJoin::BEVEL :
                out << "bevel"sv;
                break;
            case StrokeLineJoin::MITER :
                out << "miter"sv;
                break;
            case StrokeLineJoin::MITER_CLIP :
                out << "miter-clip"sv;
                break;
            case StrokeLineJoin::ROUND :
                out << "round"sv;
                break;
            default:
                out << ""sv;
                break;
        }
        return out;
    }

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeWidth(double width){
            stroke_width_ = std::move(width);
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap){
            stroke_line_cap_ = std::move(line_cap);
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join){
            stroke_line_join_ = std::move(line_join);
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;
//
            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }

//            if (fill_color_) {
//                out << " fill=\""sv;
//                visit(OstreamColorPrinter{out}, *fill_color_);
//                out << "\""sv;
//            }
//            if (stroke_color_){
//                out << " fill=\""sv;
//                visit(OstreamColorPrinter{out}, *stroke_color_);
//                out << "\""sv;
//            }
            if (stroke_width_){
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }
            if (stroke_line_cap_){
                out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
            }
            if (stroke_line_join_){
                out << " stroke-linejoin=\""sv << *stroke_line_join_  << "\""sv;
            }
        };

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
    };

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
    class Object {
    public:

        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
    class Circle final : public Object, public PathProps<Circle>{
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
    class Polyline final : public Object, public PathProps<Polyline>{
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        std::string points_ ;
        void RenderObject(const RenderContext& context) const override;
    };

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
    class Text final : public Object, public PathProps<Text> {
    public:

        Text& SetPosition(Point pos);

        Text& SetOffset(Point offset);

        Text& SetFontSize(uint32_t size);

        Text& SetFontFamily(std::string font_family);

        Text& SetFontWeight(std::string font_weight);

        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point position_ = {0.0, 0.0};
        Point offset_ = {0.0, 0.0};
        uint32_t font_size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;
    };

    // Abstract class
    class ObjectContainer{
    public:
        ObjectContainer() = default;
        ObjectContainer(const ObjectContainer& other) = delete;
        ObjectContainer(ObjectContainer&& other) = default;

        virtual ~ObjectContainer() = default;

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

        template <typename Obj>
        void Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }

    protected:

        std::deque<std::unique_ptr<Object>> objects_;
    };

    class Document: public ObjectContainer {
    public:
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */
        Document() = default;
        Document(const Document& other) = delete;
        Document(Document&& other) = default;
        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;
    };

    // Interface
    class Drawable{
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    private:
    };
}  // namespace svg
