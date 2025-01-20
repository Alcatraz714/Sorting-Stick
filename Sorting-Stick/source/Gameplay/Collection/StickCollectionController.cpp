#include "Gameplay/Collection/StickCollectionController.h"
#include "Gameplay/Collection/StickCollectionView.h"
#include "Gameplay/Collection/StickCollectionModel.h"
#include "Gameplay/GameplayService.h"
#include "Global/ServiceLocator.h"
#include "Gameplay/Collection/Stick.h"
#include <random>

namespace Gameplay
{
	namespace Collection
	{
		using namespace UI::UIElement;
		using namespace Global;
		using namespace Graphics;
		using namespace Sound;

		StickCollectionController::StickCollectionController()
		{
			collection_view = new StickCollectionView();
			collection_model = new StickCollectionModel();

			for (int i = 0; i < collection_model->number_of_elements; i++) sticks.push_back(new Stick(i));
		}

		StickCollectionController::~StickCollectionController()
		{
			destroy();
		}

		void StickCollectionController::initialize()
		{
			sort_state = SortState::NOT_SORTING;
			collection_view->initialize(this);
			initializeSticks();
			reset();
		}

		void StickCollectionController::initializeSticks()
		{
			float rectangle_width = calculateStickWidth();


			for (int i = 0; i < collection_model->number_of_elements; i++)
			{
				float rectangle_height = calculateStickHeight(i); //calc height

				sf::Vector2f rectangle_size = sf::Vector2f(rectangle_width, rectangle_height);

				sticks[i]->stick_view->initialize(rectangle_size, sf::Vector2f(0, 0), 0, collection_model->element_color);
			}
		}

		void StickCollectionController::update()
		{
			processSortThreadState();
			collection_view->update();
			for (int i = 0; i < sticks.size(); i++) sticks[i]->stick_view->update();
		}

		void StickCollectionController::render()
		{
			collection_view->render();
			for (int i = 0; i < sticks.size(); i++) sticks[i]->stick_view->render();
		}

		float StickCollectionController::calculateStickWidth()
		{
			float total_space = static_cast<float>(ServiceLocator::getInstance()->getGraphicService()->getGameWindow()->getSize().x);

			// Calculate total spacing as 10% of the total space
			float total_spacing = collection_model->space_percentage * total_space;

			// Calculate the space between each stick
			float space_between = total_spacing / (collection_model->number_of_elements - 1);
			collection_model->setElementSpacing(space_between);

			// Calculate the remaining space for the rectangles
			float remaining_space = total_space - total_spacing;

			// Calculate the width of each rectangle
			float rectangle_width = remaining_space / collection_model->number_of_elements;

			return rectangle_width;
		}

		float StickCollectionController::calculateStickHeight(int array_pos)
		{
			return (static_cast<float>(array_pos + 1) / collection_model->number_of_elements) * collection_model->max_element_height;
		}

		void StickCollectionController::updateStickPosition()
		{
			for (int i = 0; i < sticks.size(); i++)
			{
				float x_position = (i * sticks[i]->stick_view->getSize().x) + ((i + 1) * collection_model->elements_spacing);
				float y_position = collection_model->element_y_position - sticks[i]->stick_view->getSize().y;

				sticks[i]->stick_view->setPosition(sf::Vector2f(x_position, y_position));
			}
		}

		void StickCollectionController::shuffleSticks()
		{
			std::random_device device;
			std::mt19937 random_engine(device());

			std::shuffle(sticks.begin(), sticks.end(), random_engine);
			updateStickPosition();
		}

		bool StickCollectionController::compareSticksByData(const Stick* a, const Stick* b) const
		{
			return a->data < b->data;
		}

		void StickCollectionController::processSortThreadState()
		{
			if (sort_thread.joinable() && isCollectionSorted())
			{
				sort_thread.join();
				sort_state = SortState::NOT_SORTING;
			}
		}

		void StickCollectionController::inPlaceMerge(int left, int mid, int right)
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();
			int start2 = mid + 1;

			// If the direct merge is already sorted
			if (sticks[mid]->data <= sticks[start2]->data) {
				number_of_comparisons++;
				number_of_array_access += 2;
				return;
			}

			// Two pointers to maintain start of both arrays to merge
			while (left <= mid && start2 <= right) {
				number_of_comparisons++;
				number_of_array_access += 2;
				if (sticks[left]->data <= sticks[start2]->data) {
					left++;
				}
				else {
					Stick* value = sticks[start2];
					int index = start2;

					// Shift all the elements between element 1 and element 2, right by 1.
					while (index != left) {
						sticks[index] = sticks[index - 1];
						index--;
						number_of_array_access += 2;
					}
					sticks[left] = value;
					number_of_array_access++;

					// Update all the pointers
					left++;
					mid++;
					start2++;

					// Visual updates for position changes
					updateStickPosition();
				}

				// Instant color change
				sound->playSound(SoundType::COMPARE_SFX);
				sticks[left - 1]->stick_view->setFillColor(collection_model->processing_element_color);
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				sticks[left - 1]->stick_view->setFillColor(collection_model->element_color);
			}
		}

		void StickCollectionController::inPlaceMergeSort(int left, int right)
		{
			if (left < right) {
				int mid = left + (right - left) / 2;

				inPlaceMergeSort(left, mid);
				inPlaceMergeSort(mid + 1, right);
				inPlaceMerge(left, mid, right);
			}
		}

		void StickCollectionController::processMergeSort()
		{
			mergeSort(0, sticks.size() - 1);
			setCompletedColor();
		}

		void StickCollectionController::mergeSort(int left, int right)
		{
			if (left >= right) return;
			int mid = left + (right - left) / 2;

			mergeSort(left, mid);
			mergeSort(mid + 1, right);
			merge(left, mid, right);
		}

		void StickCollectionController::merge(int left, int mid, int right)
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			std::vector<Stick*> temp(right - left + 1);
			int i = left, j = mid + 1, k = 0;

			// Copy elements to the temporary array
			for (int index = left; index <= right; ++index) {
				temp[k++] = sticks[index];
				sticks[index]->stick_view->setFillColor(collection_model->temporary_processing_color);
				updateStickPosition();
			}

			i = 0;  // Start of the first half in temp
			j = mid - left + 1;  // Start of the second half in temp
			k = left;  // Start position in the original array to merge back

			// Merge elements back to the original array from temp
			while (i < mid - left + 1 && j < temp.size()) {
				if (temp[i]->data <= temp[j]->data) {
					sticks[k] = temp[i++];
				}
				else {
					sticks[k] = temp[j++];
				}

				sound->playSound(SoundType::COMPARE_SFX);
				sticks[k]->stick_view->setFillColor(collection_model->processing_element_color);
				updateStickPosition();  // Immediate update after assignment
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

				k++;
			}

			// Handle remaining elements from both halves
			while (i < mid - left + 1 || j < temp.size()) {
				if (i < mid - left + 1) {
					sticks[k] = temp[i++];
				}
				else {
					sticks[k] = temp[j++];
				}

				sound->playSound(SoundType::COMPARE_SFX);
				sticks[k]->stick_view->setFillColor(collection_model->processing_element_color);
				updateStickPosition();  // Immediate update
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

				k++;
			}
		}

		void StickCollectionController::resetSticksColor()
		{
			for (int i = 0; i < sticks.size(); i++) sticks[i]->stick_view->setFillColor(collection_model->element_color);
		}

		void StickCollectionController::resetVariables()
		{
			number_of_comparisons = 0;
			number_of_array_access = 0;
		}

		void StickCollectionController::processBubbleSort()
		{
			//ServiceLocator::getInstance()->getSoundService()->playSound(Sound::SoundType::COMPARE_SFX);
			Sound::SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int j = 0; j < sticks.size(); j++) {   // Loop through the sticks array
				if (sort_state == SortState::NOT_SORTING) { break; }   // Check if sorting has stopped or been interrupted

				bool swapped = false;  // To track if a swap was made

				for (int i = 1; i < sticks.size() - j; i++) {    // Loop through the array, reducing the range each pass
					if (sort_state == SortState::NOT_SORTING) { break; }      // Check if sorting has stopped or been interrupted

					// Increment the number of array accesses and comparisons
					number_of_array_access += 2;
					number_of_comparisons++;

					sound->playSound(Sound::SoundType::COMPARE_SFX);      // Play the compare sound effect

					// Set the current sticks to the processing color
					sticks[i - 1]->stick_view->setFillColor(collection_model->processing_element_color);
					sticks[i]->stick_view->setFillColor(collection_model->processing_element_color);

					if (sticks[i - 1]->data > sticks[i]->data) {      // Check if the current stick is greater than the next stick
						// Swap the sticks if necessary
						std::swap(sticks[i - 1], sticks[i]);
						swapped = true;  // Set swapped to true if there was a swap
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					// Reset the stick colors
					sticks[i - 1]->stick_view->setFillColor(collection_model->element_color);
					sticks[i]->stick_view->setFillColor(collection_model->element_color);

					updateStickPosition();
				}

				if (sticks.size() - j - 1 >= 0) {    // Set the last sorted stick to the placement position color
					sticks[sticks.size() - j - 1]->stick_view->setFillColor(collection_model->placement_position_element_color);
				}

				if (!swapped) { break; }    // If no swaps were made so the input was already sorted
			}
			setCompletedColor();
		}

		void StickCollectionController::processInsertionSort()
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int i = 1; i < sticks.size(); ++i)
			{

				if (sort_state == SortState::NOT_SORTING) { break; }

				int j = i - 1;
				Stick* key = sticks[i];
				number_of_array_access++; // Access for key stick


				key->stick_view->setFillColor(collection_model->processing_element_color); // Current key is red

				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

				while (j >= 0 && sticks[j]->data > key->data)
				{

					if (sort_state == SortState::NOT_SORTING) { break; }

					number_of_comparisons++;
					number_of_array_access++;

					sticks[j + 1] = sticks[j];
					number_of_array_access++; // Access for assigning sticks[j] to sticks[j + 1]
					sticks[j + 1]->stick_view->setFillColor(collection_model->processing_element_color); // Mark as being compared
					j--;
					sound->playSound(SoundType::COMPARE_SFX);
					updateStickPosition(); // Visual update

					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					sticks[j + 2]->stick_view->setFillColor(collection_model->selected_element_color); // Mark as being compared

				}

				sticks[j + 1] = key;
				number_of_array_access++;
				sticks[j + 1]->stick_view->setFillColor(collection_model->temporary_processing_color); // Placed key is green indicating it's sorted
				sound->playSound(SoundType::COMPARE_SFX);
				updateStickPosition(); // Final visual update for this iteration
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				sticks[j + 1]->stick_view->setFillColor(collection_model->selected_element_color); // Placed key is green indicating it's sorted
			}
			setCompletedColor();
		}

		void StickCollectionController::processSelectionSort()
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int i = 0; i < sticks.size() - 1; ++i)
			{

				if (sort_state == SortState::NOT_SORTING) { break; }

				int min_index = i;
				sticks[i]->stick_view->setFillColor(collection_model->selected_element_color);  // Mark the start of processing

				for (int j = i + 1; j < sticks.size(); ++j)
				{

					if (sort_state == SortState::NOT_SORTING) { break; }

					number_of_array_access += 2;
					number_of_comparisons++;

					sound->playSound(SoundType::COMPARE_SFX);
					sticks[j]->stick_view->setFillColor(collection_model->processing_element_color);
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					if (sticks[j]->data < sticks[min_index]->data)
					{
						if (min_index != i) sticks[min_index]->stick_view->setFillColor(collection_model->element_color);  // Reset previous min
						min_index = j;
						sticks[min_index]->stick_view->setFillColor(collection_model->temporary_processing_color);  // New min found
					}
					else
					{
						sticks[j]->stick_view->setFillColor(collection_model->element_color);  // Not the minimum, reset color
					}
				}

				number_of_array_access += 3;
				std::swap(sticks[min_index], sticks[i]);  // Place the found minimum at its final position

				sticks[i]->stick_view->setFillColor(collection_model->placement_position_element_color);  // Mark as sorted
				updateStickPosition();
			}

			// Ensure the last stick is also marked as sorted
			sticks[sticks.size() - 1]->stick_view->setFillColor(collection_model->placement_position_element_color);
			setCompletedColor();
		}

		void StickCollectionController::processInPlaceMergeSort(int left, int right)
		{
			inPlaceMergeSort(0, sticks.size() - 1);
			setCompletedColor();
		}

		void StickCollectionController::processQuickSort()
		{

		}

		void StickCollectionController::setCompletedColor()
		{
			for (int k = 0; k < sticks.size(); k++) {
				if (sort_state == SortState::NOT_SORTING) { break; } // Check if sorting is stoped or completed
				sticks[k]->stick_view->setFillColor(collection_model->element_color);
			}
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();
			for (int i = 0; i < sticks.size(); ++i) {
				if (sort_state == SortState::NOT_SORTING) { break; }  // Check if sorting is stoped or completed
				sound->playSound(SoundType::COMPARE_SFX);
				sticks[i]->stick_view->setFillColor(collection_model->placement_position_element_color);
				std::this_thread::sleep_for(std::chrono::milliseconds(color_delay));
			}
			// Play scream sound if sorting is still in progress
			if (sort_state == SortState::SORTING) {
				//sound->playSound(SoundType::SCREAM); - not yet present 
			}
		}

		void StickCollectionController::reset()
		{
			color_delay = 0;
			current_operation_delay = 0;
			sort_state = SortState::NOT_SORTING;
			if (sort_thread.joinable()) sort_thread.join();
			shuffleSticks();
			resetSticksColor();
			resetVariables();
		}

		void StickCollectionController::sortElements(SortType sort_type)
		{
			current_operation_delay = collection_model->operation_delay;
			color_delay = collection_model->initial_color_delay;
			this->sort_type = sort_type;
			sort_state = SortState::SORTING;

			switch (sort_type)
			{
			case Gameplay::Collection::SortType::BUBBLE_SORT:
				//sort_state = SortState::SORTING; - not needed as function begins any sorting
				time_complexity = "O(n^2)";
				sort_thread = std::thread(&StickCollectionController::processBubbleSort, this);
				break;
			case Gameplay::Collection::SortType::INSERTION_SORT:
				time_complexity = "O(n^2)"; // same method of sorting as bubble sort but sub set based
				sort_thread = std::thread(&StickCollectionController::processInsertionSort, this);
				break;
			case Gameplay::Collection::SortType::SELECTION_SORT:
				time_complexity = "O(n^2)"; // same as we still use subset based sorting like insert sort
				sort_thread = std::thread(&StickCollectionController::processSelectionSort, this);
				break;
			case Gameplay::Collection::SortType::MERGE_SORT:
				time_complexity = "O(n Log n)";
				sort_thread = std::thread(&StickCollectionController::processMergeSort, this);
				break;
			case Gameplay::Collection::SortType::QUICK_SORT:
				time_complexity = "";
				sort_thread = std::thread(&StickCollectionController::processQuickSort, this);
				break;

			}
		}

		bool StickCollectionController::isCollectionSorted()
		{
			for (int i = 1; i < sticks.size(); i++) if (sticks[i] < sticks[i - 1]) return false;
			return true;
		}

		void StickCollectionController::destroy()
		{
			current_operation_delay = 0;
			if (sort_thread.joinable()) sort_thread.join();

			for (int i = 0; i < sticks.size(); i++) delete(sticks[i]);
			sticks.clear();

			delete (collection_view);
			delete (collection_model);
		}

		SortType StickCollectionController::getSortType() { return sort_type; }

		int StickCollectionController::getNumberOfComparisons() { return number_of_comparisons; }

		int StickCollectionController::getNumberOfArrayAccess() { return number_of_array_access; }

		int StickCollectionController::getNumberOfSticks() { return collection_model->number_of_elements; }

		int StickCollectionController::getDelayMilliseconds() { return current_operation_delay; }

		sf::String StickCollectionController::getTimeComplexity() { return time_complexity; }
	}
}


