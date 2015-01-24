#include "../forwarddeclarations.h"

#define WHEEL_FRICTION_CFM 0.1
namespace troen {


	class CustomFrictionConstraint : public btTypedConstraint
	{

	protected:
		btRaycastVehicle* vehicle;

	public:

		CustomFrictionConstraint(btRigidBody& body) : btTypedConstraint(CONTACT_CONSTRAINT_TYPE, body)
		{
			m_rbA = body;
		}

		virtual ~CustomFrictionConstraint()
		{
		}

		void setVehicle(btRaycastVehicle* _vehicle)
		{
			vehicle = _vehicle;
		}

		void getInfo1(btConstraintInfo1* info)
		{
			// Add two constraint rows for each wheel on the ground
			info->m_numConstraintRows = 0;
			for (int i = 0; i < vehicle->getNumWheels(); ++i)
			{
				const btWheelInfo& wheel_info = vehicle->getWheelInfo(i);
				info->m_numConstraintRows += 2 * (wheel_info.m_raycastInfo.m_groundObject != NULL);
			}
		}

		void getInfo2(btConstraintInfo2* info)
		{
			const btRigidBody* chassis = vehicle->getRigidBody();

			int row = 0;
			// Setup sideways friction.
			for (int i = 0; i < vehicle->getNumWheels(); ++i)
			{
				const btWheelInfo& wheel_info = vehicle->getWheelInfo(i);

				// Only if the wheel is on the ground:
				if (!wheel_info.m_raycastInfo.m_groundObject)
					continue;

				int row_index = row++ * info->rowskip;

				// Set axis to be the direction of motion:
				const btVector3& axis = wheel_info.m_raycastInfo.m_wheelAxleWS;
				for (int i = 0; i < 3; i++)
					info->m_J1linearAxis[row_index + i] = axis[i];

				// Set angular axis.
				btVector3 rel_pos = wheel_info.m_raycastInfo.m_contactPointWS - chassis->getCenterOfMassPosition();
				for (int i = 0; i < 3; i++)
					info->m_J1angularAxis[row_index + i] = rel_pos.cross(axis)[i];

				// Set constraint error (target relative velocity = 0.0)
				info->m_constraintError[row_index] = 0.0f;

				info->cfm[row_index] = WHEEL_FRICTION_CFM; // Set constraint force mixing

				// Set maximum friction force according to Coulomb's law
				// Substitute Pacejka here
				btScalar max_friction = wheel_info.m_wheelsSuspensionForce * wheel_info.m_frictionSlip / info->fps;
				// Set friction limits.
				info->m_lowerLimit[row_index] = -max_friction;
				info->m_upperLimit[row_index] = max_friction;
			}

			// Setup forward friction.
			for (int i = 0; i < vehicle->getNumWheels(); ++i)
			{
				const btWheelInfo& wheel_info = vehicle->getWheelInfo(i);

				// Only if the wheel is on the ground:
				if (!wheel_info.m_raycastInfo.m_groundObject)
					continue;

				int row_index = row++ * info->rowskip;

				// Set axis to be the direction of motion:
				btVector3 axis = wheel_info.m_raycastInfo.m_wheelAxleWS.cross(wheel_info.m_raycastInfo.m_wheelDirectionWS);
				for (int i = 0; i < 3; i++)
					info->m_J1linearAxis[row_index + i] = axis[i];

				// Set angular axis.
				btVector3 rel_pos = wheel_info.m_raycastInfo.m_contactPointWS - chassis->getCenterOfMassPosition();
				for (int i = 0; i < 3; i++)
					info->m_J1angularAxis[row_index + i] = rel_pos.cross(axis)[i];

				// FIXME: Calculate the speed of the contact point on the wheel spinning.
				//        Estimate the wheel's angular velocity = m_deltaRotation
				btScalar wheel_velocity = wheel_info.m_deltaRotation * wheel_info.m_wheelsRadius;
				// Set constraint error (target relative velocity = 0.0)
				info->m_constraintError[row_index] = wheel_velocity;

				info->cfm[row_index] = WHEEL_FRICTION_CFM; // Set constraint force mixing

				// Set maximum friction force
				btScalar max_friction = wheel_info.m_wheelsSuspensionForce * wheel_info.m_frictionSlip / info->fps;
				// Set friction limits.
				info->m_lowerLimit[row_index] = -max_friction;
				info->m_upperLimit[row_index] = max_friction;
			}

		}
	};
}