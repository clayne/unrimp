/*********************************************************\
 * Copyright (c) 2012-2022 The Unrimp Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/Public/Core/Math/Math.h"
#include "Renderer/Public/Core/File/IFile.h"
#include "Renderer/Public/Core/File/IFileManager.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	// "glm::vec3" constants
	const glm::vec3 Math::VEC3_ZERO		= glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 Math::VEC3_ONE		= glm::vec3(1.0f, 1.0f, 1.0f);
	const glm::vec3 Math::VEC3_UNIT_X	= glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 Math::VEC3_RIGHT	= glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 Math::VEC3_UNIT_Y	= glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Math::VEC3_UP		= glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Math::VEC3_UNIT_Z	= glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 Math::VEC3_FORWARD	= glm::vec3(0.0f, 0.0f, 1.0f);
	// "glm::dvec3" constants
	const glm::dvec3 Math::DVEC3_ZERO	 = glm::dvec3(0.0, 0.0, 0.0);
	const glm::dvec3 Math::DVEC3_ONE	 = glm::dvec3(1.0, 1.0, 1.0);
	const glm::dvec3 Math::DVEC3_UNIT_X  = glm::dvec3(1.0, 0.0, 0.0);
	const glm::dvec3 Math::DVEC3_RIGHT	 = glm::dvec3(1.0, 0.0, 0.0);
	const glm::dvec3 Math::DVEC3_UNIT_Y  = glm::dvec3(0.0, 1.0, 0.0);
	const glm::dvec3 Math::DVEC3_UP		 = glm::dvec3(0.0, 1.0, 0.0);
	const glm::dvec3 Math::DVEC3_UNIT_Z  = glm::dvec3(0.0, 0.0, 1.0);
	const glm::dvec3 Math::DVEC3_FORWARD = glm::dvec3(0.0, 0.0, 1.0);
	// "glm::vec4" constants
	const glm::vec4 Math::VEC4_ZERO = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	const glm::vec4 Math::VEC4_ONE  = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// "glm::dvec4" constants
	const glm::dvec4 Math::DVEC4_ZERO = glm::dvec4(0.0, 0.0, 0.0, 0.0);
	const glm::dvec4 Math::DVEC4_ONE  = glm::dvec4(1.0, 1.0, 1.0, 1.0);
	// "glm::mat4" constants
	const glm::mat4 Math::MAT4_IDENTITY = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 1.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 1.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 1.0f);
	// "glm::dmat4" constants
	const glm::dmat4 Math::DMAT4_IDENTITY = glm::dmat4(1.0, 0.0, 0.0, 0.0,
													   0.0, 1.0, 0.0, 0.0,
													   0.0, 0.0, 1.0, 0.0,
													   0.0, 0.0, 0.0, 1.0);
	// "glm::quat" constants
	const glm::quat Math::QUAT_IDENTITY = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	// "glm::squat" constants
	const glm::dquat Math::DQUAT_IDENTITY = glm::dquat(1.0, 0.0, 0.0, 0.0);


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	glm::quat Math::calculateTangentFrameQuaternion(glm::mat3& tangentFrameMatrix)
	{
		// Flip y axis in case the tangent frame encodes a reflection
		const float scale = (glm::determinant(tangentFrameMatrix) > 0) ? 1.0f : -1.0f;
		tangentFrameMatrix[2][0] *= scale;
		tangentFrameMatrix[2][1] *= scale;
		tangentFrameMatrix[2][2] *= scale;

		glm::quat tangentFrameQuaternion(tangentFrameMatrix);

		{ // Make sure we don't end up with 0 as w component
			const float threshold = 1.0f / SHRT_MAX; // 16 bit quantization QTangent
			const float renomalization = sqrt(1.0f - threshold * threshold);

			if (std::abs(tangentFrameQuaternion.w) < threshold)
			{
				tangentFrameQuaternion.x *= renomalization;
				tangentFrameQuaternion.y *= renomalization;
				tangentFrameQuaternion.z *= renomalization;
				tangentFrameQuaternion.w =  (tangentFrameQuaternion.w > 0) ? threshold : -threshold;
			}
		}

		{ // Encode reflection into quaternion's w element by making sign of w negative if y axis needs to be flipped, positive otherwise
			const float qs = (scale < 0.0f && tangentFrameQuaternion.w > 0.0f) || (scale > 0.0f && tangentFrameQuaternion.w < 0.0f) ? -1.0f : 1.0f;
			tangentFrameQuaternion.x *= qs;
			tangentFrameQuaternion.y *= qs;
			tangentFrameQuaternion.z *= qs;
			tangentFrameQuaternion.w *= qs;
		}

		// Done
		return tangentFrameQuaternion;
	}

	float Math::calculateInnerBoundingSphereRadius(const glm::vec3& minimumBoundingBoxPosition, const glm::vec3& maximumBoundingBoxPosition)
	{
		// Get the minimum/maximum squared length
		const float minimumSquaredLength = glm::dot(minimumBoundingBoxPosition, minimumBoundingBoxPosition);
		const float maximumSquaredLength = glm::dot(maximumBoundingBoxPosition, maximumBoundingBoxPosition);
		ASSERT(0 != minimumSquaredLength, "Invalid minimum squared length")
		ASSERT(0 != maximumSquaredLength, "Invalid maximum squared length")

		// The greater one has to be used for the radius
		return (maximumSquaredLength > minimumSquaredLength) ? sqrt(maximumSquaredLength) : sqrt(minimumSquaredLength);
	}

	float Math::wrapToInterval(float value, float minimum, float maximum)
	{
		// Wrap as described at http://en.wikipedia.org/wiki/Wrapping_%28graphics%29
		//   value' = value - rounddown((value-min)/(max-min))*(max-min)
		// -> In here, there's no need to check for swapped minimum/maximum, it's handled correctly
		// -> Check interval in order to avoid an evil division through zero
		const float interval = (maximum - minimum);
		return interval ? (value - floor((value - minimum) / interval) * (maximum - minimum)) : minimum;
	}

	double Math::wrapToInterval(double value, double minimum, double maximum)
	{
		// Wrap as described at http://en.wikipedia.org/wiki/Wrapping_%28graphics%29
		//   value' = value - rounddown((value-min)/(max-min))*(max-min)
		// -> In here, there's no need to check for swapped minimum/maximum, it's handled correctly
		// -> Check interval in order to avoid an evil division through zero
		const double interval = (maximum - minimum);
		return interval ? (value - floor((value - minimum) / interval) * (maximum - minimum)) : minimum;
	}

	float Math::makeMultipleOf(float value, float primaryValue)
	{
		return std::floor(value / primaryValue + 0.5f) * primaryValue;
	}

	double Math::makeMultipleOf(double value, float primaryValue)
	{
		return std::floor(value / primaryValue + 0.5) * primaryValue;
	}

	uint32_t Math::makeMultipleOf(uint32_t value, uint32_t primaryValue)
	{
		return static_cast<uint32_t>(std::ceil(static_cast<float>(value) / static_cast<float>(primaryValue))) * primaryValue;
	}

	const glm::mat4& Math::getTextureScaleBiasMatrix(const Rhi::IRhi& rhi)
	{
		static const glm::mat4 SHADOW_SCALE_BIAS_MATRIX_DIRECT3D = glm::mat4(0.5f,  0.0f, 0.0f, 0.0f,
																			 0.0f, -0.5f, 0.0f, 0.0f,
																			 0.0f,  0.0f, 1.0f, 0.0f,
																			 0.5f,  0.5f, 0.0f, 1.0f);
		static const glm::mat4 SHADOW_SCALE_BIAS_MATRIX_OPENGL = glm::mat4(0.5f,  0.0f, 0.0f, 0.0f,
																		   0.0f,  0.5f, 0.0f, 0.0f,
																		   0.0f,  0.0f, 0.5f, 0.0f,
																		   0.5f,  0.5f, 0.5f, 1.0f);
		// TODO(co) Currently we just assume that "upperLeftOrigin" and "zeroToOneClipZ" RHI capabilities are always set together to the same value
		const Rhi::Capabilities& capabilities = rhi.getCapabilities();
		return (capabilities.upperLeftOrigin && capabilities.zeroToOneClipZ) ? SHADOW_SCALE_BIAS_MATRIX_DIRECT3D : SHADOW_SCALE_BIAS_MATRIX_OPENGL;
	}

	uint32_t Math::calculateFNV1a32(const uint8_t* content, uint32_t numberOfBytes, uint32_t hash)
	{
		// Sanity checks
		ASSERT(nullptr != content, "The content must be valid to be able to calculate a FNV1a32 hash")
		ASSERT(0 != numberOfBytes, "The content must be valid to be able to calculate a FNV1a32 hash")

		// 32-bit FNV-1a implementation basing on http://www.isthe.com/chongo/tech/comp/fnv/
		static constexpr uint32_t FNV1a_MAGIC_PRIME_32 = 0x1000193u;
		const uint8_t* end = content + numberOfBytes;
		for ( ; content < end; ++content)
		{
			hash = (hash ^ *content) * FNV1a_MAGIC_PRIME_32;
		}
		return hash;
	}

	uint64_t Math::calculateFNV1a64(const uint8_t* content, uint32_t numberOfBytes, uint64_t hash)
	{
		// Sanity checks
		ASSERT(nullptr != content, "The content must be valid to be able to calculate a FNV1a32 hash")
		ASSERT(0 != numberOfBytes, "The content must be valid to be able to calculate a FNV1a32 hash")

		// 64-bit FNV-1a implementation basing on http://www.isthe.com/chongo/tech/comp/fnv/
		static constexpr uint64_t FNV1a_MAGIC_PRIME_64 = 0x100000001B3u;
		const uint8_t* end = content + numberOfBytes;
		for ( ; content < end; ++content)
		{
			hash = (hash ^ *content) * FNV1a_MAGIC_PRIME_64;
		}
		return hash;
	}

	uint64_t Math::calculateFileFNV1a64ByVirtualFilename(const IFileManager& fileManager, VirtualFilename virtualFilename)
	{
		uint64_t hash = FNV1a_INITIAL_HASH_64;

		// Try open file
		IFile* file = fileManager.openFile(IFileManager::FileMode::READ, virtualFilename);
		if (nullptr != file)
		{
			// Read the file content into chunks and process them
			static constexpr uint32_t NUMBER_OF_CHUNK_BYTES = 32768;
			uint8_t chunkBuffer[NUMBER_OF_CHUNK_BYTES];
			const size_t numberOfFileBytes = file->getNumberOfBytes();
			const size_t numberOfChunks = numberOfFileBytes / NUMBER_OF_CHUNK_BYTES;
			for (size_t i = 0; i < numberOfChunks; ++i)
			{
				// We have read in a full chunk, process it
				file->read(chunkBuffer, NUMBER_OF_CHUNK_BYTES);
				hash = calculateFNV1a64(chunkBuffer, NUMBER_OF_CHUNK_BYTES, hash);
			}

			// Check if we have remaining bytes to process (less then the chunk size)
			const size_t remainingFileBytes = numberOfFileBytes - (numberOfChunks * NUMBER_OF_CHUNK_BYTES);
			if (remainingFileBytes > 0)
			{
				// Process the remaining bytes
				file->read(chunkBuffer, remainingFileBytes);
				hash = calculateFNV1a64(chunkBuffer, static_cast<uint32_t>(remainingFileBytes), hash);
			}

			// Close file
			fileManager.closeFile(*file);
		}
		else
		{
			ASSERT(false, "Failed to open the file")
		}

		// Done
		return hash;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
