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

        container.appendChild(new_element);

    });

}


fetchGroups();
