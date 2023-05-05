import { generateUUID } from "three/src/math/MathUtils.js";

export default function ChatView(props) {
    return (
        <div style={{
            zIndex: "10",
            color: "white",
            position: "absolute",
            fontFamily: "monospace",
            fontWeight: "bolder",
            left: "17%",
            top: "50%"
        }}>
            {props.chats.map((chat) =>
                <p key={generateUUID()}>{chat.id}: {chat.message}</p>
            )}
        </div>
    );
}