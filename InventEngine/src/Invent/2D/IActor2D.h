#ifndef _IACTOR2D_
#define _IACTOR2D_

#include "3D/IActor.h"

namespace INVENT
{
	class IActor2D : public IActor
	{
	public:
		IActor2D();
		virtual ~IActor2D();

		void SetColor(const glm::vec4& color) { _color = color; }
		const glm::vec4& GetColor() const { return _color; }

		virtual void SetWorldRotation(const glm::vec3& rotation);
		virtual const glm::vec3& GetWorldRotation();


	protected:


	private:
		glm::vec4 _color;

	};

}

#endif // !_IACTOR2D_

