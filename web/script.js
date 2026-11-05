async function fetchGroups() {

    try {

        const response = await fetch("/api/groups");

        if (!response.ok) {
            throw new Error(`HTTP error: ${response.status}`);
        }

        const data = await response.json();

        console.log(data);
        renderGroups(data.groups);

    } catch (error) {

        console.error("Could not fetch groups:", error);

    }

}

function renderGroups(groups) {

    const container = document.getElementById("groups-container");

    groups.forEach(function(group) {

        const new_element = document.createElement("article");

        new_element.classList.add("group-card");

        const heading = document.createElement("h3");

        const occupied = document.createElement("p");

        const slotGrid = document.createElement("div");
        slotGrid.classList.add("slot-grid");

        // Generate the 100 slot indicators
        for (let slot = 1; slot <= 100; slot++) {

            const slotElement = document.createElement("div");

            slotElement.classList.add("slot");

            // Check whether this slot contains an array
            const isOccupied = group.arrays.some(function(array) {
                return array.slot === slot;
            });

            // Apply the appropriate CSS class
            if (isOccupied) {
                slotElement.classList.add("occupied");
            } else {
                slotElement.classList.add("empty");
            }

            slotGrid.appendChild(slotElement);

        }

        occupied.textContent = `Occupied slots: ${group.occupied}`;

        const available = document.createElement("p");

        available.textContent = `Available slots: ${group.available}`;

        heading.textContent = `Group ${group.group}`;

        new_element.appendChild(heading);
        new_element.appendChild(occupied);
        new_element.appendChild(available);
        new_element.appendChild(slotGrid);

        container.appendChild(new_element);

    });

}

fetchGroups();

const form = document.getElementById("create-array-form");

form.addEventListener("submit", function(event) {

    event.preventDefault();

});
