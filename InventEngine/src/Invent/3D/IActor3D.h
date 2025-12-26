#ifndef _IACTOR3D_
#define _IACTOR3D_

#include "IActor.h"
#include "IMesh.h"

namespace INVENT
{
	class IActor3D : public IActor
	{
	public:
		IActor3D();
		virtual ~IActor3D();

		const glm::mat4& GetModelMatrice() const { return _model_matrice; }

		virtual void SetWorldRotation(const glm::vec3& rotation);
		const glm::vec3& GetWorldRotation();

		virtual void SetScale(const glm::vec3& scale);
		const glm::vec3& GetScale();

		virtual void SetWorldPosition(const glm::vec3& position) override;

	public:

		std::vector<IMesh> meshes;

	protected:
		virtual void RecalculateModelMatrix();

	private:

		glm::mat4 _model_matrice;

	};
}


#endif // !_IACTOR3D_

