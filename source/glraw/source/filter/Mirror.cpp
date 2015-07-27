#include <glraw/filter/Mirror.h>

#include <QOpenGLShaderProgram>

namespace
{
	const char * const source =
		R"(#version 150

		uniform sampler2D src;
		uniform bool horizontal;
		uniform bool vertical;

		in vec2 v_uv;
		out vec4 dst;

		void main()
		{   
			float x = horizontal	? 1 - v_uv.x : v_uv.x;
			float y = vertical		? 1 - v_uv.y : v_uv.y;
			dst = texture(src, vec2(x,y));
		} )";

	const bool DefaultHorizontal = false;
	const bool DefaultVertical = true;
}

namespace glraw
{

Mirror::Mirror( bool horizontal = DefaultHorizontal, bool vertical = DefaultVertical )
	: m_horizontal( horizontal )
	, m_vertical( vertical )
{
}

Mirror::Mirror( const QVariantMap& cfg )
	: Mirror(Get("horizontal", DefaultHorizontal, cfg), Get("vertical", DefaultVertical, cfg))
{
}

void Mirror::setUniforms(QOpenGLShaderProgram& program, unsigned int pass)
{
	program.setUniformValue( "horizontal", m_horizontal );
	program.setUniformValue( "vertical", m_vertical );
}

QString Mirror::fragmentShaderSource(unsigned int pass)
{
	return source;
}

}