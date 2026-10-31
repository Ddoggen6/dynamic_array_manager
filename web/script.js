async function fetchGroups() {

    try {

        const response = await fetch("/api/groups");

        if (!response.ok) {
            throw new Error(`HTTP error: ${response.status}`);
        }

        const data = await response.json();

        console.log(data);

    } catch (error) {

        console.error("Could not fetch groups:", error);

    }

}

fetchGroups();
