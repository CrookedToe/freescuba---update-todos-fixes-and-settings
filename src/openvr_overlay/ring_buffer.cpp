#include "ring_buffer.hpp"
#include <cstdlib>
#include <string.h>

constexpr uint8_t RING_BUFFER_VALUE_RANGE = 255;

MostCommonElementRingBuffer::MostCommonElementRingBuffer()
	: m_elements(nullptr), m_counter(nullptr), m_size(0), m_write(0), m_occupiedSize(0),
      m_mostCommonElement(0), m_mostCommonElementCount(0) {}


MostCommonElementRingBuffer::MostCommonElementRingBuffer(const uint32_t size)
	: m_elements(nullptr), m_counter(nullptr), m_size(size), m_write(0), m_occupiedSize(0),
      m_mostCommonElement(0), m_mostCommonElementCount(0)
{
	m_elements = static_cast<uint8_t*>(malloc(size * sizeof(uint8_t)));
	if (m_elements != nullptr) {
		memset(m_elements, 0xFF, size * sizeof(uint8_t));

		m_counter = static_cast<uint32_t*>(malloc(RING_BUFFER_VALUE_RANGE * sizeof(uint32_t)));
		if (m_counter != nullptr) {
			memset(m_counter, 0, RING_BUFFER_VALUE_RANGE * sizeof(uint32_t));
		}
	}
}
void MostCommonElementRingBuffer::Init(const uint32_t size)
{
	m_elements		= nullptr;
	m_counter		= nullptr;
	m_size			= size;
	m_write			= 0;
	m_occupiedSize	= 0;
	m_mostCommonElement = 0;
	m_mostCommonElementCount = 0;
	
	m_elements = static_cast<uint8_t*>(malloc(size * sizeof(uint8_t)));

	if (m_elements != nullptr) {
		memset(m_elements, 0xFF, size * sizeof(uint8_t));

		m_counter = static_cast<uint32_t*>(malloc(RING_BUFFER_VALUE_RANGE * sizeof(uint32_t)));
		if (m_counter != nullptr) {
			memset(m_counter, 0, RING_BUFFER_VALUE_RANGE * sizeof(uint32_t));
		}
	}
}

MostCommonElementRingBuffer::~MostCommonElementRingBuffer() {
	if (m_counter != nullptr) {
		free(m_counter);
		m_counter = nullptr;
	}
	if (m_elements != nullptr) {
		free(m_elements);
		m_elements = nullptr;
		m_write = -1;
	}
}

void MostCommonElementRingBuffer::Reset() {
	if (m_elements != nullptr) {
		memset(m_elements, 0, m_size * sizeof(uint8_t));
		if (m_counter != nullptr) {
			memset(m_counter, 0, RING_BUFFER_VALUE_RANGE * sizeof(uint32_t));
		}
		m_mostCommonElement = 0;
		m_mostCommonElementCount = 0;
	}
}

void MostCommonElementRingBuffer::Push(const uint8_t value) {
    if (m_elements != nullptr && m_counter != nullptr) {
        uint8_t oldValue = 0;
        bool needsRecalculation = false;

        // Increase the occupied size until we fill the buffer
        if (m_occupiedSize < m_size) {
            m_occupiedSize++;
        } else {
            // buffer is full, handle overwriting
            oldValue = m_elements[m_write];
            // Decrease last value
            m_counter[oldValue]--;

            // Check if we're removing an instance of the most common element
            if (oldValue == m_mostCommonElement) {
                m_mostCommonElementCount--;
                if (m_mostCommonElementCount == 0) {
                    needsRecalculation = true;
                }
            }
        }

        // Write element, increment counter, update the write pointer
        m_elements[m_write] = value;
        m_counter[value]++;
        m_write = (m_write + 1) % m_size;

        // Update most common element tracking
        if (value == m_mostCommonElement) {
            m_mostCommonElementCount++;
        } else if (m_counter[value] > m_mostCommonElementCount) {
            m_mostCommonElement = value;
            m_mostCommonElementCount = m_counter[value];
        } else if (needsRecalculation) {
            RecalculateMostCommonElement();
        }
    }
}

const uint8_t MostCommonElementRingBuffer::MostCommonElement() const {
    return (m_elements != nullptr && m_counter != nullptr) ? m_mostCommonElement : 0xFF;
}

void MostCommonElementRingBuffer::RecalculateMostCommonElement() {
    m_mostCommonElementCount = 0;
    m_mostCommonElement = 0;

    for (int i = 0; i < RING_BUFFER_VALUE_RANGE; i++) {
        if (m_counter[i] > m_mostCommonElementCount) {
            m_mostCommonElementCount = m_counter[i];
            m_mostCommonElement = i;
        }
    }
}