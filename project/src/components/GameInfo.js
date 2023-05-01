export default function GameInfo(props) {
    let messageElement = null;
    switch(props.message)
    {
        default: case "none":
        break;
        case "loadingworld":
        messageElement = <h4>Loading World...</h4>
    }
    return (
        <div id="gameInfo">
            {messageElement}
        </div>
    );
}