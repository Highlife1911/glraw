#include <glraw/filter/GaussianBlur.h>

#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>

#include <QOpenGLShaderProgram>


namespace
{
	const char* verticalShader =
		R"(#version 150

			uniform sampler2D src;
			uniform int size;
			uniform float[101] kernel;

			in vec2 v_uv;
			out vec4 dst;

			void main()
			{
				
				float offset = 1.0 / textureSize(src, 0).y;
				vec3 color = vec3(0.f);
				
				for (int i = -size; i <= size; ++i)
				{
					color += kernel[abs(i)]*texture(src, v_uv + vec2(0.0, i * offset)).rgb;
				}
				dst = vec4(color, texture(src, v_uv).a);
			})";

	const char* horizontalShader =
		R"(#version 150

			uniform sampler2D buf;
			uniform int size;
			uniform float[101] kernel;

			in vec2 v_uv;
			out vec4 dst;

			void main()
			{
				float offset = 1.0 / textureSize(buf, 0).x;
				dst = vec4(0.f);
				
				for (int i = -size; i <= size; ++i)
				{
					dst += kernel[abs(i)]*texture(buf, v_uv + vec2(i * offset, 0.0));
				}
			})";


	const int DefaultSize = 5;
	const float DefaultSigma = 5.f;
}

namespace glraw
{

GaussianBlur::GaussianBlur(unsigned int size = DefaultSize, float sigma = DefaultSigma)
	: m_size(VerifySize(size))
	, m_sigma(sigma)
	, m_kernel(CalculateKernel(size))
{
}

GaussianBlur::GaussianBlur(const QVariantMap& in)
	: GaussianBlur(GetSize(DefaultSize, in), Get("sigma", DefaultSigma, in))
{
}

unsigned int GaussianBlur::numberOfPasses()
{
	return 2;
}

void GaussianBlur::setUniforms(QOpenGLShaderProgram& program, unsigned int pass)
{
	program.setUniformValue("size", m_size);
	program.setUniformValueArray("kernel", m_kernel, (int)m_size + 1, 1);
}

QString GaussianBlur::fragmentShaderSource(unsigned int pass)
{
	switch (pass)
	{
	case Pass::First:
		return verticalShader;

	case Pass::Second:
		return horizontalShader;

	default:
		qCritical("Invalid pass used");
		return nullptr;
	}
}

float* GaussianBlur::CalculateKernel(unsigned int size)
{
	float *toReturn = new float[size + 1];

	float sum = 0.f;
	for (int i = 0; i < size+1;++i)
	{
		toReturn[i] = 1 / (2 * M_PI*m_sigma*m_sigma)*pow(M_E, -((i - size)*(i - size) / (2 * m_sigma*m_sigma)));
		sum += toReturn[i];
	}
	for (int i = 0; i < size + 1; ++i)
	{
		toReturn[i] /= sum;
	}

	return toReturn;
}

}